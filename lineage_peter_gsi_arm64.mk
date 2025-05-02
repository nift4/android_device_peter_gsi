# We don't inherit AOSP GSI config directly because it includes
# too many assumptions.

# All components inherited here go to system image
$(call inherit-product, $(SRC_TARGET_DIR)/product/core_64_bit.mk)

# All components inherited here go to boot image
$(call inherit-product, $(SRC_TARGET_DIR)/board/generic_arm64/device.mk)


$(call inherit-product, vendor/lineage/config/common_full_phone.mk)

# Common device config
$(call inherit-product, device/peter/gsi/device.mk)

PRODUCT_NAME := lineage_peter_gsi_arm64
PRODUCT_DEVICE := lineage_peter_gsi_arm64
PRODUCT_BRAND := Unihertz
PRODUCT_MODEL := Jelly Max
