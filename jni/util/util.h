#include <jni.h>
#include <android/log.h>
#include <unistd.h>
#include <stdlib.h>

#define DEBUG

#ifdef DEBUG
#define LOGV(...)  (__android_log_print(ANDROID_LOG_INFO,"SoSymbol",__VA_ARGS__))

#else

#define LOGV(...)

#endif
#ifdef __cplusplus
extern "C" {
#endif

const char* findLibrary(JNIEnv* env, const char* libName);

#ifdef __cplusplus
}
#endif
