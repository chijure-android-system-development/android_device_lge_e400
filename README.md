Copyright 2015 - The CyanogenMod Project

Device configuration for LGE L3
=====================================

Basic   | Spec Sheet
-------:|:-------------------------
CPU     | Single-core 800 MHz
CHIPSET | Qualcomm MSM7225a Snapdragon S1
GPU     | Adreno 200
Memory  | 384Mb RAM
Shipped Android Version | 2.3.6
Storage | 1GB
MicroSD | Up to 64GB
Battery | 1500 mAh
Dimensions | 102.6 x 61.6 x 11.9 mm
Display | 320 x 240 pixels
Camera  | 3.15 MP, 2048 x 1536 pixels
Release Date | January 2012


![LG L3 E400](http://www.lg.com/es/images/telefonos-moviles/e400-optimus-l3/gallery/lg-movil-L3-LARGE.jpg "LG L3 E400")



## CM10.1 (Android 4.2.2) on the 3.4 kernel

This branch builds CM10.1 for the E400 on top of the Qualcomm 3.4 kernel
(`chijure-android-system-development/lge-kernel-lproj`, branch
`android-msm-3.4-jb4.2-e0`). `kernel` is the prebuilt zImage of that tree.

Local manifest entry for the extra project (`.repo/local_manifests/e400.xml`):

```xml
<remote name="chijure" fetch="https://github.com/chijure-android-system-development" />
<project path="packages/apps/OtgSwitch" name="android_packages_apps_OtgSwitch"
         remote="chijure" revision="jellybean" />
```

Build: `lunch cm_e400-userdebug && make -j8 otapackage`.

### What works

Boot, WiFi, Bluetooth, speaker/headset audio, GPS HAL, sensors, zram (80 MB,
`prebuilt/30zram`), USB mass storage screen laid out for 240x320, stock LG
charging animation, USB OTG host (see below).

### Charger mode

The bootloader boots with `lge.reboot=pwroff` after a forced power-off. init
runs `/sbin/chargerwrap`, which starts the stock LG `chargerlogo` (images from
CM7, in `chargemode/`) only after a clean shutdown (`/data/misc/clean_poweroff`,
written on `sys.shutdown.requested=0`, removed once boot completes) and only
with a charger present. After a spurious reset it just reboots into Android.

### USB OTG host

The micro-USB ID pin is not seen by the SoC (`OTGSC` ID bit stays 1 with two
different OTG adapters) and the VBUS enable GPIO (141) cannot be requested, so:

- host mode is forced through debugfs: `setprop sys.otg.mode host` (or the
  `OtgSwitch` app, `packages/apps/OtgSwitch`); `peripheral` restores adb/charging
  over USB. `init.e0.rc` mounts debugfs and maps the property to
  `/sys/kernel/debug/otg/mode`.
- the phone does not power the attached device: use an OTG cable with external
  5 V.
- the kernel needs `CONFIG_USB_STORAGE=y`. `vold.fstab` mounts the drive at
  `/storage/usbdisk` (`/devices/platform/msm_hsusb_host.0`) and
  `storage_list.xml` lists it in Settings. Mouse (HID) tested, keyboard not.
- host mode disables VBUS/ID detection, so it resets on reboot.

### Known issues

- With the 3.4 kernel the phone resets silently about 610 s after boot (the
  2.6.38 kernel with the same ramdisk does not). Not caused by charger, idle,
  modem, userspace or the LGE/MSM config options tried; see the investigation
  notes in the kernel work tree.
- Camera preview works for ~10 s (gralloc `private_handle_t` ABI, 3 fds).
- A2DP, proximity sensor, FM and telephony (SIM-locked test phone) pending.
