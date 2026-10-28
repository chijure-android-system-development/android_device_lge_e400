[ROM][2.3.7][UNOFFICIAL] CyanogenMod 7 for LG Optimus L3 (E400) - stock V10c kernel, HW graphics

Hi all,

Here is an unofficial CyanogenMod 7 (Gingerbread) build for the LG Optimus L3 E400 (e0, MSM7x27A), built from a fresh device tree. The goal was not "boots and looks pretty" but to match the stock V10c behaviour (kernel, blobs, props) wherever the stock is known to work. Early test build: read the status list before flashing.

[B]Status[/B]

Working:
- Boot, reboot, power off, sleep/wake
- Display with the Adreno 200 in use (no more software rendering), auto-rotation without stripes
- Touch (multitouch), accelerometer, compass, proximity
- Audio: speaker, headset (also when plugged in before boot), voice recorder, mic
- Wi-Fi (scan, connect, browse), Bluetooth (stable address across reboots)
- Camera: photo up to 2048x1536 and HW-encoded MPEG-4 video (the E400 has a fixed-focus camera, no autofocus)
- USB mass storage, internal and external SD, off-mode charging animation (stock chargerlogo)

Partly done / untested:
- SIM / mobile network: the RIL is ported (SIM reaches READY/LOADED, IMSI is read) but I could not yet register on a network. My own Movistar SIM was rejected by the network and a SIM from another operator hits the carrier lock (the phone is e0_mov_la, Movistar). Calls, SMS and mobile data are NOT verified. I am getting another Movistar SIM to test.
- GPS: sees satellites, no fix yet
- Airplane mode, vibration, sensors beyond the basics: not tested

Not working:
- FM radio: the chip tunes, but there is no audio and the app hangs in the stock kernel's radio driver when the radio is turned off. The app is not installed in this build.
- Camera autofocus (the hardware has none)

[B]What is different from other attempts[/B]
- Stock V10c kernel (zImage) instead of a rebuilt one. A different kernel gave green frames and broken MDP.
- Stock gralloc/copybit/camera HAL blobs. The camera uses a small wrapper over the stock LG HAL.
- The stock RIL blobs. Some of the telephony blobs I started from came from another firmware and were replaced with the V10c ones.
- Everything not in the ROM source is kept as small, reviewable patches in the device tree (patches/apply-patches.sh).

[B]Installation[/B]
1. Back up everything (at least /data and the boot partition).
2. Boot a custom recovery (I used TWRP).
3. Wipe data/factory reset, cache and dalvik (first install from stock needs it).
4. Flash the zip, wipe cache and dalvik again, reboot. First boot takes about a minute.

[B]Download[/B]
[DOWNLOAD LINK]
MD5: 601ff46fbf7c8806db1a2557293f6b1e (cm-7-20261028-UNOFFICIAL-e400.zip, 83.6 MB)

[B]Sources[/B]
- Device tree (branch gingerbread): https://github.com/chijure-android-system-development/android_device_lge_e400
- Vendor blobs: https://github.com/chijure/e400
- docs/E400_NOTES.md in the device tree has the build notes, root causes and the comparison with the stock ROM.

[B]Notes for developers[/B]
- A few things cost me days and may save you some: ueventd on this device loads ueventd.lge.rc (it uses the first word of cpuinfo "Hardware"), so ueventd.e0.rc is never read; debug.composition.type=mdp is not optional with the stock gralloc; HeadsetObserver must read /sys/class/switch/h2w_headset; and the card status of the stock RIL carries three subscription indexes.
- Bug reports are welcome with logcat (adb logcat -b radio for telephony) and dmesg.

[B]Credits[/B]
CyanogenMod team, Code Aurora Forum, the people who worked on other MSM7x27A Gingerbread ports (the Huawei Y210 / Y220 trees were the model for the HAL wrappers), and everyone who tested.

Disclaimer: unofficial build, provided as is. Flashing can brick your device and void your warranty. Nobody but you is responsible for what happens to your phone.
