# Notas de bring-up — LG L3 E400 (e0, MSM7x27A), CM7

Estado al 2026-10-28. Basado en pruebas reales en el dispositivo (E400g, stock V10c
2.3.6 de origen). Complementa el README (lista de pruebas).

## 1. Compilar

Contenedor `cm7-builder` en **Docker Engine** (no Docker Desktop: la VM de Desktop
tiene solo ~11 GB y un bind mount lento). Desde la raíz del árbol:

```
bash device/lge/e400/patches/apply-patches.sh        # árbol limpio -> parches del E400
sg docker -c "docker --context default exec cm7-builder bash -lc '
  cd ~/cm7 && export USER=builder BUILD_NUMBER=userdebug.builder USE_CCACHE=1 \
       CCACHE_DIR=/home/builder/.ccache
  . build/envsetup.sh && lunch cyanogen_e400-userdebug && make -j8 bacon'"
```

- **No usar `breakfast`**: `roomservice` crea `.repo/local_manifests/roomservice.xml` y
  apunta `device/lge/e400` al repo de CyanogenMod (un `repo sync` lo pisaría).
- `USER` y `BUILD_NUMBER` hacen falta: sin ellos el zip se llama `...-eng..zip` y
  `vendor/cyanogen/tools/squisher` busca `...-userdebug..zip` y falla al final.
- Si una librería cambia de origen (fuente -> blob) hay que borrar la copia vieja en
  `out/target/product/e400/{system/lib,obj/lib,symbols/system/lib}`: `make` no
  reemplaza un archivo más nuevo que su fuente.

## 2. Flashear

- TWRP 4.4.4. El zip va a `/emmc` (en recovery `/sdcard` es RAM).
- **No encadenar `twrp wipe ...` justo después de `twrp install ...`**: TWRP responde
  `Another threaded action is already running` y los wipes no corren.
- Para probar cambios de librerías es más rápido `adb push` + reiniciar `mediaserver`
  o el framework que flashear el zip completo (~3 min).
- Los dumps de pantalla (`/dev/graphics/fb0`) pesan ~30 MB por serie; `/data` tiene
  160 MB, no dejarlos acumular.

## 3. Cambios fuera de este repo

Viven en `patches/` y se aplican con `apply-patches.sh` (como el Y210):

| Parche | Para qué |
|---|---|
| `frameworks_base_camera.patch` | gancho genérico `BOARD_CAMERA_CLIENT_EXTRA_SRC` en `libs/camera/Android.mk`; las 20 constantes de `CameraParameters` que importa el HAL del stock viven en `libcamera_compat/` |
| `frameworks_base_surfaceflinger_fb_stride.patch` | capturas del framebuffer con el stride real (rayas al rotar) |
| `frameworks_base_wifi_statetracker.patch` | no bajar `wlan0` al desconectar y subirla antes de escanear |
| `external_wpa_supplicant_8_e400.patch` | stub `wpa_ctrl_cleanup()` + `SIOCSIWFREQ` no fatal (driver Libra) |
| `packages_apps_FileManager_proguard.patch` | ProGuard de userdebug aborta por `android.support.v2` |
| `vendor_cyanogen_cyanogen_e400.mk` | una línea que incluye `device/lge/e400/cyanogen_e400.mk`: `CM_BUILD` fuerza `vendor/cyanogen/products/cyanogen_<dev>.mk` |

Lo que **ya no necesita parche** porque vive en el dispositivo: el producto
(`cyanogen_e400.mk`) y `get_rfc3526_prime_1536` para `wpa_supplicant`
(`wpa_supplicant_8_lib/bn_const_compat.c`).

`vendor/lge/e400/e400-vendor-blobs.mk` (repo de blobs) necesita estas líneas:

```
vendor/lge/e400/proprietary/lib/libcamera.so:system/lib/libcamera.e400.so
vendor/lge/e400/proprietary/lib/libOmxCore.so:system/lib/libOmxCore.so
vendor/lge/e400/proprietary/lib/libmm-omxcore.so:system/lib/libmm-omxcore.so
```

## 4. Problemas resueltos y su causa

| Síntoma | Causa | Arreglo |
|---|---|---|
| `mediaserver` caía al escanear/reproducir video | faltaban `libOmxCore.so` y `libmm-omxcore.so`; `OMXMaster.cpp:92 err != OMX_ErrorNoMore` | blobs añadidos |
| ROM lenta, CPU al 90 % | `qmiproxy` en bucle: no existía `/data/radio` porque `init.e0.rc` usaba `on post-fs-data`, que **el init de GB no dispara** | `on post-fs` (como el stock) |
| Wi-Fi lista redes pero no asocia | el driver Libra devuelve `EOPNOTSUPP` a `SIOCSIWFREQ` y `wpa_supplicant_8` lo trataba como fatal | no fatal |
| Cámara: `mediaserver` SIGSEGV | el HAL fuente exigía `mmcamera_camframe_callback`, que `liboemcamera.so` de LG no exporta | wrapper sobre el HAL del stock |
| Carga con el teléfono apagado no funciona | el `chargerlogo` del árbol (84 664 B, de otro kernel de LG: usa `/sys/module/pm2/parameters/hidden_resetting` y `/data/nv/FRStatus.txt`) **terminaba en segfault** (`exit=139`) al ejecutarlo; sus imágenes también eran de otro juego (fotogramas de pantalla completa en vez de recortes) | `chargerlogo` e imágenes del stock V10c (el binario del stock corre su bucle de animación sin fallar) y `exec /sbin/chargerlogo` como en el stock |
| Wi-Fi no detecta ninguna red tras desconectar/olvidar una red (`wlan0` se cae cada ~5 s) | `WifiStateTracker` llama a `NetworkUtils.disableInterface()` al pasar a DISCONNECTED y nadie la vuelve a subir: los escaneos corren con la interfaz abajo (`wifi doCommand: ret=-1` cada 6 s). El log lo delata: `WifiStateTracker: Disabling interface` justo tras `Scan requested` | `patches/frameworks_base_wifi_statetracker.patch` (el mismo del Y220): no bajar la interfaz y subirla antes de escanear |
| Rayas horizontales ~0,4 s al rotar con el sensor | `SurfaceFlinger::directCaptureScreenImplLocked` (y el efecto de apagado) copian el framebuffer suponiendo filas de `xres*bpp` = 480 B, pero el stride real del msm_fb es 512 B; la captura que usa la animación de rotación salía desplazada 32 B por fila | `patches/frameworks_base_surfaceflinger_fb_stride.patch`: usa `line_length` |
| Pantalla verde ~0,3 s al abrir Cámara; `mdp_ppp: invalid image` | **el kernel del árbol** (`#2 Oct 8 2012`, "Manhattan") | kernel del stock V10c (`#1 Aug 8 2012`) |

### Cámara (sensor `hi351`)

`libcamera/E400CameraWrapper.cpp` es el `libcamera.so`; carga el HAL del stock como
`libcamera.e400.so` y llama a cada método por el slot real de su vtable (CM7 y el
blob difieren: `getShutterSound` = 4, `getBufferInfo` = 10, `encodeData` = 11,
`takePicture` sin argumentos). Trampas:

1. El linker de Gingerbread resuelve los símbolos de un `dlopen` solo en las
   dependencias del propio blob, por eso las constantes de `CameraParameters` deben
   estar en `libcamera_client` (parche) y no en el wrapper.
2. El `CameraInfo` del blob es `{facing, modos, orientation}` (12 bytes) y el de CM7
   mide 8: el wrapper usa un buffer propio. Sin esto desborda y la app falla en
   `setDisplayOrientation`.
3. `HAL_openCameraHardware(id, mode)`: el blob rechaza `mode` 0; se pasa 1 (2D).

Validado: preview, foto 2048x1536, video MPEG-4 640x480 con encoder de hardware.
Autofocus no soportado por el HAL. En poca luz el sensor baja a ~5-8 fps (el HAL
solo acepta el rango `5000,30000`).

### Kernel

`kernel` es el `zImage` del stock V10c, extraído de
`boot.emmc.win` del backup de TWRP del stock
(`/mnt/fuentes/backups/lg-e400g-healthy-stock-20261027/`). Con él, 3 corridas de
apertura de Cámara dieron 0 cuadros verdes y 0 errores `mdp_ppp` (con el anterior:
10 cuadros verdes y errores del MDP en cada rotación).

## 5. Comparación con el stock (V10c)

- Blobs gráficos (Adreno, `gralloc.msm7k`, `copybit.msm7k`, `egl.cfg`): idénticos.
- `libsurfaceflinger.so` del stock es una variante CAF de Qualcomm (bypass, overlay,
  composición MDP) y **no es compatible con el CM7** (`libsystem_server` no enlaza).
- `libpixelflinger` del stock trae `rotate90CW_4x4_16v6` (rotación 90° NEON); el de CM7 no.
- Stock: `debug.composition.7x27A.type=gpu`. `debug.composition.type=mdp` de nuestro
  `system.prop` no lo lee nada en CM7 (herencia de CM9).

## 6. Problemas abiertos

- ~~Rayas horizontales al rotar con el sensor~~: **resuelto** con el parche de stride de SurfaceFlinger (0 cuadros con rayas de 320 grabados, antes 87-97). Diagnóstico: `strace` de `system_server` mostró un `open` + `FBIOGET_VSCREENINFO` por cada rotación (la captura directa) y las rayas tenían la firma de un desplazamiento de 32 B por fila (suave por fila, discontinuo entre filas). `copybit` no tenía nada que ver.
  **Las librerías de framework del stock no se pueden intercambiar** con las de CM7 (`libsurfaceflinger`, `libGLES_android`, `libpixelflinger`): el framework cae (pantalla blanca y luego negra). Si pasa, recuperar con `adb root`, no con `su`.
- Animación de rotación "fea": probablemente era lo mismo que las rayas; pendiente confirmarlo con el usuario.
- Video con poca luz: bajo fps y borroso (límite del sensor/HAL, sin verificar con luz).
- Tras el cambio de kernel, verificado a mano: touch, audio, Bluetooth, rotación y
  carga con el teléfono apagado OK. Sin probar: vibración, sensores y modo avión.

## 7. Cómo diagnosticar gráficos desde adb

```
# 24+ cuadros del framebuffer (RGB565, 240x320, stride 512, 3 páginas; pan = y)
dd if=/dev/graphics/fb0 of=f$i.raw bs=512 skip=$(cut -d, -f2 /sys/class/graphics/fb0/pan) count=320
dmesg | grep mdp_ppp            # blits rechazados por el MDP
dumpsys SurfaceFlinger          # capas, formatos y stride
```

Un valor de píxel constante `0x07E0` en toda la pantalla es el verde puro.

## 8. Auditoría del árbol (2026-10-28)

Se quitó lo que no se compila, no se instala o nadie lee. Verificado con una
compilación completa y comparando el md5 de los 673 archivos de `system` y los 26
del ramdisk contra el build anterior: solo cambian `init.e0.rc` y `build.prop`
(los `.jar` se reempaquetan con otra fecha).

| Quitado | Por qué |
|---|---|
| `libcamera/QualcommCameraHardware.*`, `msm_camera.h`, `include/mm-camera`, `include/mm-still` | HAL de cámara fuente ya sin uso (~350 llamadas de log) |
| `libgralloc/`, `libcopybit/` | definen `gralloc.e400`/`copybit.e400`, que no se compilan; se usan los blobs `*.msm7k` |
| `configs/` `e0_keypad.kl`, `touch_mcs8000.kl/.idc`, `7k_handset.kl`, `7x27a_kp.kcm.bin`, `qwerty.idc/.kcm.bin`, `nfcee_access.xml`; `prebuilt/scsi_wait_scan.ko` | ningún `.mk` los instala; herencia de CM9. El stock tampoco instala `e0_keypad.kl` ni `touch_mcs8000.kl` |
| `libmm-omxcore`, `libOmxCore` en `PRODUCT_PACKAGES` | no hay módulos con ese nombre; vienen como blobs |
| 10 propiedades de `system.prop` | sin lector en CM7, en los blobs de LG ni en el stock |
| 6 comandos repetidos de `init.e0.rc` | `mkdir` de `init` solo hace `chown` si crea el directorio |

Se dejó a propósito: `UsbController.cpp` (lo usa `BOARD_CUSTOM_USB_CONTROLLER`),
`include/linux/msm_mdp.h` y `include/camera/CameraParameters.h`
(`TARGET_SPECIFIC_HEADER_PATH`), `setprop wifi.interface` (está en dos secciones
distintas) y el segundo `mkdir /data/misc/gpsone_d` (otro propietario; nunca llega a
aplicarse, pero no es un duplicado exacto, conviene decidir cuál es el correcto).

Trampas del build que salieron al verificarlo:

- `build.prop` no se regenera al cambiar `system.prop`: borrar
  `out/target/product/e400/system/build.prop` antes de compilar.
- Si cambia la lista de fuentes de una librería (p. ej. `libcrypto`), `make` no la
  relinkea: borrar `obj/.../LINKED/<lib>.so`, `obj/lib/<lib>.so`, `system/lib/<lib>.so`.
- Ruido de log: el stock es más ruidoso que el CM7 (1 942 contra 1 543 líneas en el
  arranque). Lo que más sobra en CM7 es `ONCRPC` (blob de Qualcomm, sin interruptor).

### Carga con el teléfono apagado

`init` ejecuta `/sbin/chargerlogo` en `on boot-pause`, que se dispara cuando el
bootloader pasa `lge.reboot=pwroff` en la línea de comandos
(`CHARGERMODE_CMDLINE_NAME/VALUE` en `BoardConfig.mk`). Los archivos de
`chargemode/` son los del stock V10c (kernel `#1 Aug 8 2012`). Probar el binario sin
apagar el teléfono: `adb root; /sbin/chargerlogo` y mirar su salida (el stock imprime
`write fb success` / `fb_update success`; un segfault es `exit=139`). **Verificado por el usuario**: con el teléfono apagado y el cargador conectado se muestra
la animación de carga (el bootloader pasa `lge.reboot=pwroff`).

