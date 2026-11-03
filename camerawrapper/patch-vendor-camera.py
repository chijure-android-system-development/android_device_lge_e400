import struct, sys, hashlib
src, dst = sys.argv[1], sys.argv[2]
d = bytearray(open(src,'rb').read())
# (vaddr, expected halfword): ldr rX,[rY,#32|#36] on private_handle_t; blob expects 3-fd layout
sites = [(0x1aa30,0x6a11),(0x1aa54,0x6a30),(0x1aa5a,0x6a71),(0x1ae48,0x6a4b),
         (0x1ae6e,0x6a08),(0x1ae9e,0x6a08),(0x1aed0,0x6a11),(0x1c04e,0x6a16),(0x1c788,0x6a11)]
for va, exp in sites:
    h = struct.unpack_from('<H', d, va)[0]
    assert h == exp, (hex(va), hex(h))
    struct.pack_into('<H', d, va, h - 0x40)   # imm5 - 1  => offset - 4
open(dst,'wb').write(d)
print(hashlib.md5(d).hexdigest())
