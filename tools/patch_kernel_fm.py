#!/usr/bin/env python3
"""patch_kernel_fm.py <stock zImage> <patched zImage>

Fixes a deadlock in the stock V10c kernel's radio-tavarua driver.
tavarua_handle_interrupts() holds radio->lock and, when the chip raises the
ERROR interrupt bit (INTSTAT3 & 0x80, seen after every FM turn-on), calls
sync_read_xfr(ERROR_CODE) for a debug print. sync_read_xfr -> xfr_intf_own
takes radio->lock again, so the irq worker blocks on itself and every later
FM ioctl hangs in D state (reboot needed).

Patch: the `bge` at 0xc030bd5c that skips the debug block when the ERROR bit
is clear becomes an unconditional `b`, so the block is never entered. The
recompressed gzip payload is exactly as long as the stock one, so the zImage
layout (decompressor, trailer) is untouched.
"""
import hashlib, re, struct, subprocess, sys, zlib

STOCK_MD5 = "349ba92d33f9ab861afe79092f023053"
VADDR = 0xc030bd5c          # tavarua_handle_interrupts+0x884 in the V10c kernel
KBASE = 0xc0008000
OLD, NEW = 0xaa000008, 0xea000008   # bge +8  ->  b +8

src, dst = sys.argv[1:3]
d = bytearray(open(src, "rb").read())
if hashlib.md5(d).hexdigest() != STOCK_MD5:
    sys.exit("input is not the stock V10c zImage")

o = re.search(b"\x1f\x8b\x08", bytes(d)).start()
dec = zlib.decompressobj(16 + zlib.MAX_WBITS)
img = bytearray(dec.decompress(bytes(d[o:])))
n = len(d) - o - len(dec.unused_data)

off = VADDR - KBASE
if struct.unpack("<I", img[off:off + 4])[0] != OLD:
    sys.exit("unexpected instruction at the patch site")
img[off:off + 4] = struct.pack("<I", NEW)

gz = subprocess.run(["gzip", "-9", "-n", "-c"], input=bytes(img),
                    stdout=subprocess.PIPE, check=True).stdout
if len(gz) != n:
    sys.exit("recompressed payload is %d bytes, stock is %d" % (len(gz), n))
d[o:o + n] = gz
assert zlib.decompressobj(16 + zlib.MAX_WBITS).decompress(bytes(d[o:])) == bytes(img)
open(dst, "wb").write(d)
print("ok: %s md5 %s" % (dst, hashlib.md5(d).hexdigest()))
