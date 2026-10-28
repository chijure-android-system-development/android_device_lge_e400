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

# frameworks/base — gancho genérico BOARD_CAMERA_CLIENT_EXTRA_SRC en libs/camera:
#   deja compilar dentro de libcamera_client las constantes de CameraParameters que
#   importa el libcamera.so del stock (están en libcamera_compat/ del dispositivo).
apply frameworks/base frameworks_base_camera.patch

# frameworks/base — SurfaceFlinger: las capturas directas del framebuffer (animación
#   de rotación y apagado de pantalla) usaban xres como ancho de fila; el stride
#   real del msm_fb es 512 B (256 px) y salían cuadros con rayas horizontales.
apply frameworks/base frameworks_base_surfaceflinger_fb_stride.patch

# frameworks/base — WifiStateTracker: no bajar wlan0 al pasar a DISCONNECTED (los
#   escaneos siguientes se quedaban con la interfaz abajo y Ajustes no listaba redes)
#   y subirla antes de cada escaneo. Mismo parche que usa el Y220.
apply frameworks/base frameworks_base_wifi_statetracker.patch

# system/core — init: permitir persist.service.bt.* al usuario bluetooth, como el init
#   del stock. hci_qcomm_init guarda ahí su dirección; sin la regla se negaba y el
#   Bluetooth cambiaba de dirección (y perdía los emparejamientos) en cada arranque.
apply system/core system_core_property_service.patch

# external/wpa_supplicant_8 — stub wpa_ctrl_cleanup() (lo llama el wifi.c de GB)
#   y SIOCSIWFREQ no fatal (el driver Libra devuelve EOPNOTSUPP).
apply external/wpa_supplicant_8 external_wpa_supplicant_8_e400.patch

# frameworks/base — FM: JNI Qualcomm tavarua (BOARD_FM_DEVICE=qcom) + stack Java
#   android.hardware.fmradio (mismo que el Y210; el chip es el mismo 0x4020205).
apply frameworks/base frameworks_base_fm.patch
copy_new frameworks_base_android_hardware_fm_qcom.cpp frameworks/base/core/jni/android_hardware_fm_qcom.cpp
apply frameworks/base frameworks_base_fm_java.patch

# packages/apps/FM — app FM para el stack tavarua (config en overlay/packages/apps/FM).
apply packages/apps/FM packages_apps_FM.patch

# packages/apps/FileManager — ProGuard falla en userdebug por android.support.v2.
apply packages/apps/FileManager packages_apps_FileManager_proguard.patch

# vendor/cyanogen — CM_BUILD (build/core/product_config.mk) importa siempre
#   vendor/cyanogen/products/cyanogen_<dispositivo>.mk; ahí solo va una línea que
#   incluye el producto real, que vive en device/lge/e400/cyanogen_e400.mk.
copy_new vendor_cyanogen_cyanogen_e400.mk vendor/cyanogen/products/cyanogen_e400.mk

echo "OK"
