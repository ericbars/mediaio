LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_MODULE := ogg
LOCAL_SRC_FILES := prebuilt/libogg.so
include $(PREBUILT_SHARED_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := vorbis
LOCAL_SRC_FILES := prebuilt/libvorbis.so
include $(PREBUILT_SHARED_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := vorbisenc
LOCAL_SRC_FILES := prebuilt/libvorbisenc.so
include $(PREBUILT_SHARED_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := mp3lame
LOCAL_SRC_FILES := prebuilt/libmp3lame.so
include $(PREBUILT_SHARED_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := x264
LOCAL_SRC_FILES := prebuilt/libx264.so
include $(PREBUILT_SHARED_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := x265
LOCAL_SRC_FILES := prebuilt/libx265.so
include $(PREBUILT_SHARED_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := ffmpeg
LOCAL_SRC_FILES := prebuilt/libffmpeg.so
include $(PREBUILT_SHARED_LIBRARY)

include $(CLEAR_VARS)
LOCAL_C_INCLUDES += $(LOCAL_PATH)/include
LOCAL_MODULE := transcode
LOCAL_SRC_FILES := transcode.cpp cmdutils.c ffmpeg_filter.c ffmpeg_opt.c ffmpeg_hw.c ffmpeg.c ffprobe.c

# for logging
LOCAL_LDLIBS += -llog
LOCAL_SHARED_LIBRARIES := libffmpeg
LOCAL_SHARED_LIBRARIES += libmp3lame
LOCAL_SHARED_LIBRARIES += libx264
LOCAL_SHARED_LIBRARIES += libx265
LOCAL_SHARED_LIBRARIES += libogg
LOCAL_SHARED_LIBRARIES += libvorbis
LOCAL_SHARED_LIBRARIES += libvorbisenc

LOCAL_CFLAGS += -D__STDC_CONSTANT_MACROS=1
LOCAL_CFLAGS += -D__STDC_FORMAT_MACROS=1

include $(BUILD_SHARED_LIBRARY)