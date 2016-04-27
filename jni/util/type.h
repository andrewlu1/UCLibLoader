#ifndef __TYPE__H__
#define __TYPE__H__
/**
 * 用来处理函数签名内容.
 */
#include <jni.h>
#include <stdio.h>

//用来缓存类型查找数据.以及类中方法数据.
typedef struct _CacheNode {
	char className[100];
	jclass clazz;
	jobjectArray methods;

	struct _CacheNode* next;
} CacheNode, *PCacheNode;

jclass FindClass(JNIEnv* env, const char* className);
const char* GetMethodSign(JNIEnv* env, const char* className,
		const char* methodName);
void ClearCache(JNIEnv* env);
#endif
