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

# Overlay
PRODUCT_PACKAGE_OVERLAYS += device/peter/gsi/overlay

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

# vendor/foss
PRODUCT_PACKAGES += \
    UnifiedNlp

# ThemePicker (for Launcher3)
PRODUCT_PACKAGES += \
    ThemePicker

# Enable ro.adb.secure on userdebug and user
ifeq (,$(filter eng,$(TARGET_BUILD_VARIANT)))
    PRODUCT_SYSTEM_EXT_PROPERTIES += \
        ro.adb.secure=1
endif
