LOCAL_PATH := device/peter/gsi/quirks

# Keylayout quirks
PRODUCT_COPY_FILES += \
    $(LOCAL_PATH)/keylayout/unihertz-fingerprint_key.kl:$(TARGET_COPY_OUT_SYSTEM_EXT)/quirks/keylayout/unihertz-fingerprint_key.kl \
    $(LOCAL_PATH)/keylayout/unihertz-mtk-kpd.kl:$(TARGET_COPY_OUT_SYSTEM_EXT)/quirks/keylayout/unihertz-mtk-kpd.kl \
    $(LOCAL_PATH)/keylayout/unihertz-mtk-tpd.kl:$(TARGET_COPY_OUT_SYSTEM_EXT)/quirks/keylayout/unihertz-mtk-tpd.kl \
