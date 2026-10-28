#!/bin/bash
# apply-patches.sh — aplica los parches del E400 sobre un árbol CM7 limpio.
# Ejecutar desde la raíz del árbol de CM7:
#   bash device/lge/e400/patches/apply-patches.sh
#
# Cada parche se verifica con `git apply --check` antes de aplicarse; si algo
# ya está aplicado el script se detiene sin dejar cambios a medias.

set -e

PATCHES_DIR="$(dirname "$0")"
CM7_ROOT="$(pwd)"

apply() {
    local project="$1"
    local patch="$2"
    echo "  Aplicando $patch en $project..."
    (cd "$CM7_ROOT/$project" && git apply --check "$CM7_ROOT/$PATCHES_DIR/$patch" \
        && git apply "$CM7_ROOT/$PATCHES_DIR/$patch")
}

copy_new() {
    local src="$1"
    local dst="$2"
    echo "  Copiando archivo nuevo: $dst"
    cp "$CM7_ROOT/$PATCHES_DIR/$src" "$CM7_ROOT/$dst"
}

# frameworks/base — cámara: CameraParameters (HFR/denoise/redeye/MCE/YV12) que
#   importa el libcamera.so del stock (libcamera.e400.so) desde libcamera_client.
apply frameworks/base frameworks_base_camera.patch

# frameworks/base — SurfaceFlinger: las capturas directas del framebuffer (animación
#   de rotación y apagado de pantalla) usaban xres como ancho de fila; el stride
#   real del msm_fb es 512 B (256 px) y salían cuadros con rayas horizontales.
apply frameworks/base frameworks_base_surfaceflinger_fb_stride.patch

# frameworks/base — WifiStateTracker: no bajar wlan0 al pasar a DISCONNECTED (los
#   escaneos siguientes se quedaban con la interfaz abajo y Ajustes no listaba redes)
#   y subirla antes de cada escaneo. Mismo parche que usa el Y220.
apply frameworks/base frameworks_base_wifi_statetracker.patch

# external/openssl — libcrypto debe exportar get_rfc3526_prime_1536 (bn_const.c)
#   para enlazar wpa_supplicant_8 (WPA_SUPPLICANT_VERSION := VER_0_8_X).
apply external/openssl external_openssl_bn_const.patch

# external/wpa_supplicant_8 — stub wpa_ctrl_cleanup() (lo llama el wifi.c de GB)
#   y SIOCSIWFREQ no fatal (el driver Libra devuelve EOPNOTSUPP).
apply external/wpa_supplicant_8 external_wpa_supplicant_8_e400.patch

# packages/apps/FileManager — ProGuard falla en userdebug por android.support.v2.
apply packages/apps/FileManager packages_apps_FileManager_proguard.patch

# vendor/cyanogen — producto cyanogen_e400 (lo usa `lunch cyanogen_e400-userdebug`).
apply vendor/cyanogen vendor_cyanogen_e400_product.patch
copy_new vendor_cyanogen_cyanogen_e400.mk vendor/cyanogen/products/cyanogen_e400.mk

echo "OK"
