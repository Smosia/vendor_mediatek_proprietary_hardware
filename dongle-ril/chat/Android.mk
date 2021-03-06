ifeq ($(MTK_EXTERNAL_DONGLE_SUPPORT),yes)

LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

TARGET_PRELINK_MODULE := false

LOCAL_SRC_FILES:= \
	chat.c

LOCAL_SHARED_LIBRARIES := \
	libcutils \
	libcrypto \
	libutils


LOCAL_CFLAGS := -DANDROID_CHANGES -DCHAPMS=1 -DMPPE=1 -DDEBUGALL -Iexternal/openssl/include

LOCAL_MODULE:= chat
LOCAL_PROPRIETARY_MODULE := true
LOCAL_MODULE_OWNER := mtk

include $(MTK_EXECUTABLE)

endif #(($(MTK_EXTERNAL_DONGLE_SUPPORT),yes)

