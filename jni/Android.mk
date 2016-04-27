LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_MODULE := bootstrap
LOCAL_LDLIBS +=  -llog
LOCAL_SRC_FILES :=  util/util.cpp util/type.cpp so/SoSymbols.cpp core/OverrideDlopen.cpp bootstrap.cpp
include $(BUILD_SHARED_LIBRARY)


# #以下代码用来进行测试
#include $(CLEAR_VARS)  
#LOCAL_MODULE := utils
#LOCAL_SRC_FILES := libutils.so  
#include $(PREBUILT_SHARED_LIBRARY)  
#
#include $(CLEAR_VARS)
#LOCAL_MODULE := mmath
#LOCAL_LDLIBS +=  -llog
#LOCAL_SHARED_LIBRARIES :=utils
#LOCAL_SRC_FILES :=  math.c
#include $(BUILD_SHARED_LIBRARY)

