# Include GSM stuff
$(call inherit-product, vendor/cyanogen/products/gsm.mk)

# Inherit device configuration for e400.
$(call inherit-product, device/lge/e400/e400.mk)

# Inherit some common cyanogenmod stuff.
$(call inherit-product, vendor/cyanogen/products/common_full_no_themes.mk)

#
# Setup device specific product configuration.
#
PRODUCT_NAME := cyanogen_e400
PRODUCT_BRAND := lge
PRODUCT_DEVICE := e400
PRODUCT_MODEL := LG-E400
PRODUCT_MANUFACTURER := LGE
PRODUCT_BUILD_PROP_OVERRIDES += PRODUCT_NAME=e0_mov_la BUILD_ID=GRK39F BUILD_FINGERPRINT=lge/e0_mov_la/e0:2.3.6/GRK39F/V10c-AUG-07-2012.2ED8EFAB36:user/release-keys PRIVATE_BUILD_DESC="e0_mov_la-user 2.3.6 GRK39F V10c-AUG-07-2012.2ED8EFAB36 release-keys"

PRODUCT_PACKAGE_OVERLAYS += vendor/cyanogen/overlay/ldpi

# Release name and versioning
PRODUCT_RELEASE_NAME := L3
PRODUCT_VERSION_DEVICE_SPECIFIC :=
-include vendor/cyanogen/products/common_versions.mk

#
# Copy specific prebuilt files
#
PRODUCT_COPY_FILES +=  \
    vendor/cyanogen/prebuilt/ldpi/media/bootanimation.zip:system/media/bootanimation.zip
