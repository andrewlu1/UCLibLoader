#include "util.h"

const char* findLibrary(JNIEnv* env, const char* libName) {

	static jclass VMStack = env->FindClass("dalvik/system/VMStack");
	static jmethodID getSystemClassLoader = env->GetStaticMethodID(VMStack,
			"getCallingClassLoader", "()Ljava/lang/ClassLoader;");
	static jclass baseClassLoaderC = env->FindClass(
			"dalvik/system/BaseDexClassLoader");

	static jmethodID findLibrary = env->GetMethodID(baseClassLoaderC,
			"findLibrary", "(Ljava/lang/String;)Ljava/lang/String;");

	jobject classLoader = env->CallStaticObjectMethod(VMStack,
			getSystemClassLoader);

	jstring libNameStr = env->NewStringUTF(libName);
	jstring libPath = (jstring) env->CallObjectMethod(classLoader, findLibrary,
			libNameStr);

	env->DeleteLocalRef(libNameStr);

	if (libPath == NULL) {
		LOGV("libPath== NULL");
		return NULL;
	}
	return env->GetStringUTFChars(libPath, 0);
}
