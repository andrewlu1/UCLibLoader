#include <jni.h>
/**
 * 编写另一个简单模块用于代码验证.
 * @author liuzw
 */

JNIEXPORT jint JNI_OnLoad(JavaVM* vm, void *reserved) {
	return JNI_VERSION_1_6;
}

JNIEXPORT jint JNICALL Java_com_uc_base_system_Test_hello(JNIEnv* env,
		jclass thiz, jint a, jint b) {
	return a + b;
}

	JNIEXPORT jlong JNICALL Java_com_uc_base_system_Test_max(JNIEnv* env,
			jobject thiz, jint a, jlong b) {
		return a > b ? a : b;
	}
