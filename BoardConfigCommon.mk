DEVICE_PATH := device/peter/gsi

# Bluetooth
BOARD_BLUETOOTH_BDROID_BUILDCFG_INCLUDE_DIR := $(DEVICE_PATH)/bluetooth

# Props
TARGET_SYSTEM_PROP := $(DEVICE_PATH)/system.prop

# Sepolicy
SYSTEM_EXT_PRIVATE_SEPOLICY_DIRS += $(DEVICE_PATH)/sepolicy/private

# GSI specific System Properties
# We override the one from BoardConfigGsiCommon.mk, because we don't
# really want ro.adb.secure=0 on userdebug.
ifneq (,$(filter eng,$(TARGET_BUILD_VARIANT)))
TARGET_SYSTEM_EXT_PROP := build/make/target/board/gsi_system_ext.prop
else
TARGET_SYSTEM_EXT_PROP := build/make/target/board/gsi_system_ext_user.prop
endif

# GMS build
BUILD_BROKEN_ELF_PREBUILT_PRODUCT_COPY_FILES := true

# Reserve 150MB of empty space in system
BOARD_SYSTEMIMAGE_PARTITION_RESERVED_SIZE := 157286400
BOARD_SYSTEMIMAGE_EXTFS_INODE_COUNT := 32768
