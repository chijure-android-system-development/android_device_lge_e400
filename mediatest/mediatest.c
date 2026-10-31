#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <linux/media.h>
int main(void) {
    char path[32];
    int n;
    for (n = 0; n < 4; n++) {
        struct media_device_info info;
        struct media_entity_desc ent;
        int fd, id;
        snprintf(path, sizeof(path), "/dev/media%d", n);
        fd = open(path, O_RDWR | O_NONBLOCK);
        if (fd < 0) { printf("%s: open: %s\n", path, strerror(errno)); continue; }
        memset(&info, 0, sizeof(info));
        if (ioctl(fd, MEDIA_IOC_DEVICE_INFO, &info) < 0) { printf("%s: DEVICE_INFO: %s\n", path, strerror(errno)); close(fd); continue; }
        printf("%s: driver='%s' model='%s' bus='%s'\n", path, info.driver, info.model, info.bus_info);
        id = 0;
        for (;;) {
            memset(&ent, 0, sizeof(ent));
            ent.id = id | MEDIA_ENT_ID_FLAG_NEXT;
            if (ioctl(fd, MEDIA_IOC_ENUM_ENTITIES, &ent) < 0) { printf("  enum end: %s\n", strerror(errno)); break; }
            printf("  ent id=%u name='%s' type=0x%x group_id=%d\n", ent.id, ent.name, ent.type, ent.group_id);
            id = ent.id;
        }
        close(fd);
    }
    return 0;
}
