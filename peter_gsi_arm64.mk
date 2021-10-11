# We don't inherit AOSP GSI config directly because it includes
# too many assumptions.

# All components inherited here go to system image
$(call inherit-product, $(SRC_TARGET_DIR)/product/core_64_bit.mk)

# All components inherited here go to boot image
$(call inherit-product, $(SRC_TARGET_DIR)/board/generic_arm64/device.mk)

# Common device config
$(call inherit-product, device/peter/gsi/device.mk)

PRODUCT_NAME := peter_gsi_arm64
PRODUCT_DEVICE := peter_gsi_arm64
PRODUCT_BRAND := Android
PRODUCT_MODEL := AOSP on ARM64
