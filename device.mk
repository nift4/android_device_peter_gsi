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
PRODUCT_SOONG_NAMESPACES += device/peter/gsi

# Reuiqred for system-side HALs like sysbta
# TODO: Can we work around this?
SELINUX_IGNORE_NEVERALLOWS := true

# Overlay
PRODUCT_PACKAGES += \
    GsiFrameworkResTarget \
    GsiLauncher3Target

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

# vendor/foss
include vendor/foss/foss.mk

# Launcher3
PRODUCT_PACKAGES += \
    Launcher3QuickStep \
    ThemePicker

# Calculator from Lineage (packages/apps/ExactCalculator)
PRODUCT_PACKAGES += \
    ExactCalculator

# Bluetooth Audio (System-side HAL, sysbta)
PRODUCT_PACKAGES += \
    audio.sysbta.default \
    android.hardware.bluetooth.audio-service-system

PRODUCT_COPY_FILES += \
    device/peter/gsi/bluetooth/audio/config/sysbta_audio_policy_configuration.xml:$(TARGET_COPY_OUT_SYSTEM)/etc/sysbta_audio_policy_configuration.xml \
    device/peter/gsi/bluetooth/audio/config/sysbta_audio_policy_configuration_7_0.xml:$(TARGET_COPY_OUT_SYSTEM)/etc/sysbta_audio_policy_configuration_7_0.xml

# QcRilAm service for QCOM in-call audio (packages/apps/QcRilAm)
PRODUCT_PACKAGES += \
    QcRilAm

# Enable ro.adb.secure on userdebug and user
ifeq (,$(filter eng,$(TARGET_BUILD_VARIANT)))
    PRODUCT_SYSTEM_EXT_PROPERTIES += \
        ro.adb.secure=1
endif

# Custom commands
PRODUCT_PACKAGES += \
    aguiledbeltctl
