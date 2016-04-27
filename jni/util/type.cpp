#include "type.h"
#include <string.h>
#include "util.h"

static PCacheNode allocNode() {
	PCacheNode p = new CacheNode;
	p->className[0] = '\0';
	p->clazz = NULL;
	p->methods = NULL;
	return p;
}

//头节点不为空.但也不存内容.
static PCacheNode pHeader = allocNode();

/**
 * 从缓存中找到一个记录类型信息的缓存节点.
 */
static PCacheNode FindCacheNode(JNIEnv* env, const char* className) {
	PCacheNode p = pHeader;
	while (p != NULL) {
		if (strcmp(p->className, className) == 0) {
			return p;
		}
		if (p->next == NULL)
			break;
		p = p->next;
	}
	//添加一个节点.并返回.
	PCacheNode q = allocNode();

	strcpy(q->className, className);
	q->clazz = env->FindClass(className);
	q->next = NULL;
	p->next = q;

	//获取此类中的所有函数.
	static jclass c = env->FindClass("java/lang/Class");
	static jmethodID getDeclaredmethods = env->GetMethodID(c,
			"getDeclaredMethods", "()[Ljava/lang/reflect/Method;");
	jobjectArray methods = (jobjectArray) env->CallObjectMethod(q->clazz,
			getDeclaredmethods);
	q->methods = methods;

	return q;
}
/**
 * 通过ClassName 在缓存链表中查找缓存数据. 如果末找到则同时生成类型信息并缓存 .
 */
jclass FindClass(JNIEnv* env, const char* className) {
	PCacheNode p = FindCacheNode(env, className);
	LOGV("FindClass result :%d", (p!=NULL));
	return p->clazz;
}
/**
 * 获取方法对应的签名. 通常情况下我们应该人工传入签名.但这里为了省事,用代码生成签名信息.
 * 但有一个限制,JNI接口不能有重载函数. 否则就无法根据函数名字找到一个唯一的函数及签名.
 */
const char* GetMethodSign(JNIEnv* env, const char* className,
		const char* methodName) {

	//Method类中有一个方法 getSignature 可以获取到方法签名.
	jclass Method = env->FindClass("java/lang/reflect/Method");
	static jmethodID getSignature = env->GetMethodID(Method, "getSignature",
			"()Ljava/lang/String;");
	static jmethodID getName = env->GetMethodID(Method, "getName",
			"()Ljava/lang/String;");

	LOGV("GetMethodSign result :%s", methodName);
	PCacheNode p = FindCacheNode(env, className);
	jclass clazz = p->clazz;
	jobjectArray methods = p->methods;
	int size = env->GetArrayLength(methods);

	for (int i = 0; i < size; i++) {
		jobject method = env->GetObjectArrayElement(methods, i);
		jstring name = (jstring) env->CallObjectMethod(method, getName);
		const char* nameChars = env->GetStringUTFChars(name, 0);
		if (strcmp(nameChars, methodName) == 0) {
			jstring sign = (jstring) env->CallObjectMethod(method,
					getSignature);
			if (sign != NULL)
				return env->GetStringUTFChars(sign, 0);
			break;
		}
	}

	return NULL;
}
/**
 * 缓存链表的清理. 无法保证何时应该清理,因为无法估计用户何时loadLibrary.所以只能留到Unload本库的时候再做清理.
 */
void ClearCache(JNIEnv* env) {
	PCacheNode p = pHeader;
	PCacheNode q = NULL;
	while (p != NULL) {
		q = p->next;
		delete (p);
		p = q;
	}
}
