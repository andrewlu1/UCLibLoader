/*
 * SoSymbols.h
 *
 *  Created on: 2016-4-26
 *      Author: Administrator
 */

#ifndef SOSYMBOLS_H_
#define SOSYMBOLS_H_

#include <jni.h>
#include <stdio.h>
#include <string.h>
#include "../util/util.h"
#include "../util/elf_def.h"
#include "../util/type.h"

typedef jint (*JNI_OnLoad_M)(JavaVM* vm, void *reserved);

class LibReader {
public:
	LibReader();
	~LibReader();
	void Init(const char* libPath);
	char** GetSymbolTable();
	int GetSymbolSize();
private:
	bool isInited;
	FILE* fp;Elf_Ehdr libHdr;
	char* symbolStrAddr;
	char** symbolTable; //char[][];
	int symbolSize;
};

//处理Java_jni函数的注册过程.
bool registerNatives(JNIEnv* env, const char* libPath);

//处理某一符号对应的注册过程.
//void resolveSymbol(JNIEnv* env, void* handler, char* symbol);

#endif /* SOSYMBOLS_H_ */
