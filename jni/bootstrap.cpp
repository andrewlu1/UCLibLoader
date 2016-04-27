#include<jni.h>
#include<dlfcn.h>
#include <stdio.h>
#include <stdlib.h>

#include "util/util.h"
#include "so/SoSymbols.h"
#include "core/OverrideDlopen.h"
#include "util/type.h"

#ifndef __INTERFACE__H__
#define __INTERFACE__H__

#ifdef __cplusplus
extern "C" {
#endif

JNIEXPORT jint JNI_OnLoad(JavaVM* vm, void *reserved) {
	LOGV("JNI OnLoad Entry!");
	JNIEnv* env = NULL;
	jint result = -1;
	if (vm->GetEnv((void**) &env, JNI_VERSION_1_4) != JNI_OK) {
		LOGV("ERROR: GetEnv failed\n");
		return JNI_ERR;
	}
	uc_dlopenEnsureInit();

	LOGV("JNI OnLoad Exit!");
	return JNI_VERSION_1_4;
}

JNIEXPORT void Java_com_uc_base_system_UCLibLoader_nativeLoadLibrary(
		JNIEnv* env, jobject thiz, jstring libName) {

	const char* libNameC = env->GetStringUTFChars(libName, 0);
	const char* fullLibName = findLibrary(env, libNameC);

	if (fullLibName == NULL) {
		LOGV("JNI can not find Library:%s", libNameC);
		return;
	}

	dlerror();
	bool ret = registerNatives(env, fullLibName);
	if (ret) {
		LOGV("JNI onLoad custom lib success.");
	} else {
		const char* error = dlerror();
		LOGV("JNI onLoad custom lib failed:%s", error);
	}
}

JNIEXPORT void JNICALL JNI_OnUnload(JavaVM* vm, void* reserved) {
	LOGV("JNI_OnUnload Entry!");
	JNIEnv* env = NULL;
	jint result = -1;
	if (vm->GetEnv((void**) &env, JNI_VERSION_1_4) != JNI_OK) {
		LOGV("ERROR: GetEnv failed\n");
		return;
	}
	ClearCache(env);
}
#ifdef __cplusplus
}
#endif

#endif
