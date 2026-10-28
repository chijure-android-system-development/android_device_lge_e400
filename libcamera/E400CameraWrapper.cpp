#define LOG_TAG "E400CameraWrapper"

#include <dlfcn.h>
#include <string.h>

#include <cutils/properties.h>
#include <utils/Log.h>
#include <camera/CameraParameters.h>

#include "E400CameraWrapper.h"

namespace android {

// The CameraParameters statics the stock HAL imports come from libcamera_client
// (see patches/frameworks_base_camera.patch).

typedef sp<CameraHardwareInterface> (*OpenCamFunc)(int, int);
typedef void (*GetCamInfoFunc)(int, struct CameraInfo*);
typedef int (*GetNumCamerasFunc)();

static OpenCamFunc gOpenCameraHardware = NULL;
static GetCamInfoFunc gGetCameraInfo = NULL;
static GetNumCamerasFunc gGetNumberOfCameras = NULL;
static void *gLibHandle = NULL;
static void *gOemHandle = NULL;

// HAL_openCameraHardware(sp<>* ret, cameraId, mode): the blob rejects mode 0
// and wants 1 (2D), as LG's libcameraservice passes.
static const int kCameraMode = 1;

// Slot numbers in the stock blob's vtable (vptr[n], vptr[0] = ~D1).
//   CM7 order  : ... setCallbacks 4 ... startPreview 8, useOverlay 9 ...
//   Stock order: getShutterSound 4, getBufferInfo 10, encodeData 11 inserted.
enum {
    SLOT_getPreviewHeap = 2,
    SLOT_getRawHeap = 3,
    SLOT_setCallbacks = 5,
    SLOT_enableMsgType = 6,
    SLOT_disableMsgType = 7,
    SLOT_msgTypeEnabled = 8,
    SLOT_startPreview = 9,
    SLOT_useOverlay = 12,
    SLOT_setOverlay = 13,
    SLOT_stopPreview = 14,
    SLOT_previewEnabled = 15,
    SLOT_startRecording = 16,
    SLOT_stopRecording = 17,
    SLOT_recordingEnabled = 18,
    SLOT_releaseRecordingFrame = 19,
    SLOT_autoFocus = 20,
    SLOT_cancelAutoFocus = 21,
    SLOT_takePicture = 22,
    SLOT_cancelPicture = 23,
    SLOT_setParameters = 24,
    SLOT_getParameters = 25,
    SLOT_sendCommand = 26,
    SLOT_release = 27,
    SLOT_dump = 28
};

static inline void* slot(const sp<CameraHardwareInterface>& iface, int n)
{
    return (*reinterpret_cast<void***>(iface.get()))[n];
}

#define BLOB(ret, n, sig) \
    reinterpret_cast<ret (*) sig>(slot(mLib, SLOT_##n))

static void ensureLibOpened()
{
    if (gLibHandle != NULL) {
        return;
    }
    // The blob dlopen()s this itself during createInstance(); do it first and
    // globally so that does not depend on our linker namespace.
    gOemHandle = ::dlopen("liboemcamera.so", RTLD_NOW | RTLD_GLOBAL);
    if (gOemHandle == NULL) {
        LOGE("dlopen(liboemcamera.so) failed: %s", dlerror());
    }
    gLibHandle = ::dlopen("libcamera.e400.so", RTLD_NOW);
    if (gLibHandle == NULL) {
        LOGE("dlopen(libcamera.e400.so) failed: %s", dlerror());
        return;
    }
    gOpenCameraHardware = reinterpret_cast<OpenCamFunc>(
            ::dlsym(gLibHandle, "HAL_openCameraHardware"));
    gGetCameraInfo = reinterpret_cast<GetCamInfoFunc>(
            ::dlsym(gLibHandle, "HAL_getCameraInfo"));
    gGetNumberOfCameras = reinterpret_cast<GetNumCamerasFunc>(
            ::dlsym(gLibHandle, "HAL_getNumberOfCameras"));
    LOGI("stock HAL entry points: open=%p info=%p num=%p",
            gOpenCameraHardware, gGetCameraInfo, gGetNumberOfCameras);
}

extern "C" int HAL_getNumberOfCameras()
{
    ensureLibOpened();
    return gGetNumberOfCameras ? gGetNumberOfCameras() : 0;
}

extern "C" void HAL_getCameraInfo(int cameraId, struct CameraInfo* cameraInfo)
{
    ensureLibOpened();
    if (gGetCameraInfo != NULL) {
        // The stock blob's CameraInfo is { facing, supported modes,
        // orientation } (12 bytes); CM7's is { facing, orientation }.
        int raw[4] = { 0, 0, 0, 0 };
        gGetCameraInfo(cameraId, reinterpret_cast<struct CameraInfo*>(raw));
        cameraInfo->facing = raw[0];
        cameraInfo->orientation = raw[2];
    }
}

extern "C" sp<CameraHardwareInterface> HAL_openCameraHardware(int cameraId)
{
    return E400CameraWrapper::createInstance(cameraId);
}

sp<CameraHardwareInterface> E400CameraWrapper::createInstance(int cameraId)
{
    ensureLibOpened();
    if (gOpenCameraHardware == NULL) {
        return NULL;
    }
    sp<CameraHardwareInterface> lib = gOpenCameraHardware(cameraId, kCameraMode);
    if (lib == NULL) {
        LOGE("stock HAL rejected cameraId=%d", cameraId);
        return NULL;
    }
    return new E400CameraWrapper(lib);
}

E400CameraWrapper::E400CameraWrapper(sp<CameraHardwareInterface> lib)
    : mLib(lib)
{
}


E400CameraWrapper::~E400CameraWrapper()
{
}

sp<IMemoryHeap> E400CameraWrapper::getPreviewHeap() const
{
    if (mLib == NULL) return NULL;
    return BLOB(sp<IMemoryHeap>, getPreviewHeap, (void*))(mLib.get());
}

sp<IMemoryHeap> E400CameraWrapper::getRawHeap() const
{
    if (mLib == NULL) return NULL;
    return BLOB(sp<IMemoryHeap>, getRawHeap, (void*))(mLib.get());
}

void E400CameraWrapper::setCallbacks(notify_callback notify_cb,
        data_callback data_cb, data_callback_timestamp data_cb_timestamp,
        void *user)
{
    if (mLib == NULL) return;
    BLOB(void, setCallbacks, (void*, notify_callback, data_callback,
            data_callback_timestamp, void*))(mLib.get(), notify_cb, data_cb,
            data_cb_timestamp, user);
}

void E400CameraWrapper::enableMsgType(int32_t msgType)
{
    if (mLib == NULL) return;
    BLOB(void, enableMsgType, (void*, int32_t))(mLib.get(), msgType);
}

void E400CameraWrapper::disableMsgType(int32_t msgType)
{
    if (mLib == NULL) return;
    BLOB(void, disableMsgType, (void*, int32_t))(mLib.get(), msgType);
}

bool E400CameraWrapper::msgTypeEnabled(int32_t msgType)
{
    if (mLib == NULL) return false;
    return BLOB(bool, msgTypeEnabled, (void*, int32_t))(mLib.get(), msgType);
}

status_t E400CameraWrapper::startPreview()
{
    if (mLib == NULL) return INVALID_OPERATION;
    return BLOB(status_t, startPreview, (void*))(mLib.get());
}

bool E400CameraWrapper::useOverlay()
{
    // Preview through the preview heap; overlay stays an opt-in for bring-up.
    char value[PROPERTY_VALUE_MAX];
    property_get("debug.camera.use_overlay", value, "0");
    return strcmp(value, "1") == 0;
}

status_t E400CameraWrapper::setOverlay(const sp<Overlay> &overlay)
{
    if (mLib == NULL || !useOverlay()) return INVALID_OPERATION;
    return BLOB(status_t, setOverlay, (void*, const sp<Overlay>&))(mLib.get(),
            overlay);
}

void E400CameraWrapper::stopPreview()
{
    if (mLib == NULL) return;
    BLOB(void, stopPreview, (void*))(mLib.get());
}

bool E400CameraWrapper::previewEnabled()
{
    if (mLib == NULL) return false;
    return BLOB(bool, previewEnabled, (void*))(mLib.get());
}

status_t E400CameraWrapper::startRecording()
{
    if (mLib == NULL) return INVALID_OPERATION;
    return BLOB(status_t, startRecording, (void*))(mLib.get());
}

void E400CameraWrapper::stopRecording()
{
    if (mLib == NULL) return;
    BLOB(void, stopRecording, (void*))(mLib.get());
}

bool E400CameraWrapper::recordingEnabled()
{
    if (mLib == NULL) return false;
    return BLOB(bool, recordingEnabled, (void*))(mLib.get());
}

void E400CameraWrapper::releaseRecordingFrame(const sp<IMemory> &mem)
{
    if (mLib == NULL) return;
    BLOB(void, releaseRecordingFrame, (void*, const sp<IMemory>&))(mLib.get(),
            mem);
}

status_t E400CameraWrapper::autoFocus()
{
    if (mLib == NULL) return INVALID_OPERATION;
    return BLOB(status_t, autoFocus, (void*))(mLib.get());
}

status_t E400CameraWrapper::cancelAutoFocus()
{
    if (mLib == NULL) return INVALID_OPERATION;
    return BLOB(status_t, cancelAutoFocus, (void*))(mLib.get());
}

status_t E400CameraWrapper::takePicture()
{
    if (mLib == NULL) return INVALID_OPERATION;
    return BLOB(status_t, takePicture, (void*))(mLib.get());
}

status_t E400CameraWrapper::cancelPicture()
{
    if (mLib == NULL) return INVALID_OPERATION;
    return BLOB(status_t, cancelPicture, (void*))(mLib.get());
}

status_t E400CameraWrapper::dump(int fd, const Vector<String16> &args) const
{
    if (mLib == NULL) return INVALID_OPERATION;
    return BLOB(status_t, dump, (void*, int, const Vector<String16>&))(
            mLib.get(), fd, args);
}

status_t E400CameraWrapper::setParameters(const CameraParameters& params)
{
    if (mLib == NULL) return INVALID_OPERATION;
    return BLOB(status_t, setParameters, (void*, const CameraParameters&))(
            mLib.get(), params);
}

CameraParameters E400CameraWrapper::getParameters() const
{
    if (mLib == NULL) return CameraParameters();
    return BLOB(CameraParameters, getParameters, (void*))(mLib.get());
}

status_t E400CameraWrapper::sendCommand(int32_t command, int32_t arg1,
        int32_t arg2)
{
    if (mLib == NULL) return INVALID_OPERATION;
    return BLOB(status_t, sendCommand, (void*, int32_t, int32_t, int32_t))(
            mLib.get(), command, arg1, arg2);
}

void E400CameraWrapper::release()
{
    if (mLib == NULL) return;
    BLOB(void, release, (void*))(mLib.get());
    mLib.clear();
}

}; // namespace android
