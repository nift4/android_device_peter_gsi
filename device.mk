# All components inherited here go to system image
$(call inherit-product, $(SRC_TARGET_DIR)/product/generic_system.mk)

# All components inherited here go to system_ext image
$(call inherit-product, $(SRC_TARGET_DIR)/product/handheld_system_ext.mk)
$(call inherit-product, $(SRC_TARGET_DIR)/product/telephony_system_ext.mk)
$(call inherit-product, device/generic/common/gsi_system_ext.mk)

# All components inherited here go to product image
$(call inherit-product, $(SRC_TARGET_DIR)/product/aosp_product.mk)
$(call inherit-product, device/generic/common/gsi_product.mk)

# Special settings for GSI releasing
$(call inherit-product, $(SRC_TARGET_DIR)/product/gsi_release.mk)

# This is whitelisted by product name -- unset it
PRODUCT_INSTALL_DEBUG_POLICY_TO_SYSTEM_EXT :=

# Enable dynamically-sized images
PRODUCT_USE_DYNAMIC_PARTITION_SIZE := true

# Soong namespaces
PRODUCT_SOONG_NAMESPACES += device/peter/gsi hardware/mediatek

# Reuiqred for system-side HALs like sysbta
# TODO: Can we work around this?
SELINUX_IGNORE_NEVERALLOWS := true
TARGET_FACE_UNLOCK_OPTOUT := true
# Overlay
PRODUCT_PACKAGES += \
    GsiFrameworkResTarget \
    GsiLauncher3Target \
    GsiSettingsTarget

# Device-specific overlays from phhusson
include vendor/hardware_overlay/overlay.mk

# Init
PRODUCT_PACKAGES += \
    init_gsi

# Quirks
include device/peter/gsi/quirks/quirks.mk

# Euicc -- enabled by default, won't work without eSIM slot definition anyway
PRODUCT_COPY_FILES += \
    frameworks/native/data/etc/android.hardware.telephony.euicc.xml:$(TARGET_COPY_OUT_PRODUCT)/etc/permissions/android.hardware.telephony.euicc.xml

PRODUCT_PACKAGES += \
    OpenEUICC

# Telecom API (Phone Accounts, etc.)
PRODUCT_COPY_FILES += \
    frameworks/native/data/etc/android.software.telecom.xml:$(TARGET_COPY_OUT_PRODUCT)/etc/permissions/android.software.telecom.xml
PRODUCT_BOOT_JARS += \
    system_ext:mediatek-common \
    system_ext:mediatek-framework \
    system_ext:mediatek-ims-base \
    system_ext:mediatek-ims-common \
    system_ext:mediatek-telecom-common \
    system_ext:mediatek-telephony-base \
    system_ext:mediatek-telephony-common


# vendor/foss
include vendor/foss/foss.mk

# Bluetooth Audio (System-side HAL, sysbta)
PRODUCT_PACKAGES += \
    audio.sysbta.default \
    android.hardware.bluetooth.audio-service-system

PRODUCT_COPY_FILES += \
    device/peter/gsi/bluetooth/audio/config/sysbta_audio_policy_configuration.xml:$(TARGET_COPY_OUT_SYSTEM)/etc/sysbta_audio_policy_configuration.xml \
    device/peter/gsi/bluetooth/audio/config/sysbta_audio_policy_configuration_7_0.xml:$(TARGET_COPY_OUT_SYSTEM)/etc/sysbta_audio_policy_configuration_7_0.xml

# Enable ro.adb.secure on userdebug and user
ifeq (,$(filter eng,$(TARGET_BUILD_VARIANT)))
    PRODUCT_SYSTEM_EXT_PROPERTIES += \
        ro.adb.secure=1
endif

# Custom commands
PRODUCT_PACKAGES += \
    aguiledbeltctl \
    aguiauxcam \
    aguidt2w \
    FMRadio \
    Aperture \
    PowerOffAlarm

PRODUCT_COPY_FILES += \
    device/peter/gsi/mtk-pmic-keys.kl:$(TARGET_COPY_OUT_SYSTEM)/usr/keylayout/mtk-pmic-keys.kl
# Iwlan (VoWiFi)
PRODUCT_PACKAGES += \
    Iwlan

$(call inherit-product, vendor/peter/gsi/gsi-vendor.mk)

