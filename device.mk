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

# Enable dynamically-sized images
PRODUCT_USE_DYNAMIC_PARTITION_SIZE := true

# Relaxed artifact requirements (mirror AOSP GSI)
PRODUCT_ENFORCE_ARTIFACT_PATH_REQUIREMENTS := relaxed

# Soong namespaces
PRODUCT_SOONG_NAMESPACES += device/peter/gsi

# Overlay
PRODUCT_PACKAGE_OVERLAYS += device/peter/gsi/overlay

# Device-specific overlays from phhusson
include vendor/hardware_overlay/overlay.mk

# Init
PRODUCT_PACKAGES += \
    init_gsi

