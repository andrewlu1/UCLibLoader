/*
 * SoSymbols.h
 *
 *  Created on: 2016-4-26
 *      Author: Administrator
 */

#include "Sosymbols.h"
#include "../core/OverrideDlopen.h"
#include <dlfcn.h>
#include <string.h>
#include "../util/type.h"

#define FILE_INVALIDE NULL

LibReader::LibReader() :
		isInited(false), fp(FILE_INVALIDE), symbolStrAddr(NULL), symbolTable(
				NULL), symbolSize(0) {
}
LibReader::~LibReader() {
	if (symbolStrAddr != NULL) {
		free((void*) symbolStrAddr);
		symbolStrAddr = NULL;
	}
	if (symbolTable != NULL) {
		free(symbolTable);
		symbolTable = NULL;
	}
	LOGV("LibReader dellocated!");
}

void LibReader::Init(const char* libPath) {
	if (isInited)
		return;
	LOGV("LibReader inited start.");

	fp = fopen(libPath, "rb");
	if (!fp)
		return;
	Elf_Shdr strSHdr;

	fread(&libHdr, sizeof(libHdr), 1, fp);

	//magic number.
	if (memcmp(libHdr.e_ident, ELFMAG, SELFMAG) != 0) {
		LOGV("LibReader check magic error!");
		fclose(fp);
		return;
	}

	//跳到节开始的位置.开始逐个读取所有的节数据.
	fseek(fp, libHdr.e_shoff, SEEK_SET);
	for (int i = 0; i < libHdr.e_shnum; i++) {
		if (fread(&strSHdr, sizeof(strSHdr), 1, fp) != 1) {
			//error.
			LOGV("LibReader read section header error.");
			fclose(fp);
			return;
		}

		//读取到了字符串节.并且不是节名字符串节.
		if (SHT_STRTAB == strSHdr.sh_type && i != libHdr.e_shstrndx) {
			symbolStrAddr = (char*) malloc(strSHdr.sh_size);
			fseek(fp, strSHdr.sh_offset, SEEK_SET);
			fread(symbolStrAddr, sizeof(char), strSHdr.sh_size, fp);
			break;
		}
	}

	fclose(fp);
	fp = FILE_INVALIDE;

	if (symbolStrAddr == NULL)
		return;
	int count = 1, size = 0;
	while (count < strSHdr.sh_size) {
		count += strlen(symbolStrAddr + count++);
		size++;
	}
	symbolSize = size;
	symbolTable = (char**) malloc(size * sizeof(void*));
	count = 1;
	for (int i = 0; i < size; i++) {
		symbolTable[i] = symbolStrAddr + count;
		count += strlen(symbolStrAddr + count);
		count++;
	}

	LOGV("LibReader inited success!");
	isInited = true;
}

char** LibReader::GetSymbolTable() {
	if (!isInited)
		return NULL;

	return symbolTable;
}

int LibReader::GetSymbolSize() {
	return symbolSize;
}
//============================================================

//============================================================
static void resolveSymbol(JNIEnv* env, void* handler, char* symbol);
//注册此库中出现的函数.
bool registerNatives(JNIEnv* env, const char* libPath) {
	void* handler = uc_dlopen(libPath, RTLD_LAZY);
	JNI_OnLoad_M onLoad = (JNI_OnLoad_M) dlsym(handler, "JNI_OnLoad");
	if (onLoad != NULL) {
		JavaVM* jvm;
		env->GetJavaVM(&jvm);
		onLoad(jvm, NULL);
		LOGV("JNI_OnLoad Method ret:%d", jvm!=NULL);
	} else {
		return false;
	}

	LibReader reader;
	reader.Init(libPath);
	//char* [] 存放了函数符号数组.
	char** symbols = reader.GetSymbolTable();
	int symbolSize = reader.GetSymbolSize();

	for (int i = 0; i < symbolSize; i++) {
		//先简单过滤一下: 开头不是Java_的全部忽略.
		if (symbols[i] != NULL && symbols[i][0] != 'J') {
			continue;
		}
		resolveSymbol(env, handler, symbols[i]);
	}
	return true;
}

static void resolveSymbol(JNIEnv* env, void* handler, char* symbol) {
	LOGV("GetSymbol:%s", symbol);
	if (symbol == NULL)
		return;
	//查找以Java_开头的函数名.
	if (strstr(symbol, "Java_") != symbol) {
		return;
	}
	char symbolCpy[100] = { };
	strcpy(symbolCpy, symbol);

	const char* splitChar = "_";
	char* result = strtok(symbolCpy, splitChar); //这个分割方法会改变字符串内容.因此需要备份一个.
	char* symbolData[20]; //如:{"Java","com","uc","base","system","Test","add"}
	char className[250] = { }; //"com/uc/base/system/Test"
	int i = 0;
	while (result != NULL) {
		symbolData[i++] = result;
		result = strtok(NULL, splitChar);
	}
	//拼接出一个完整的类名.
	int j = 1;
	for (; j < i - 1; j++) {
		strcat(className, symbolData[j]);
		if (j < i - 2) {
			strncat(className, "/", 1); //换成/分割符.
		}
	}
	//这里需要对类的查找,方法查找等过程进行缓存,否则降低效率 .
	jclass clazz = FindClass(env, className); //FindClass(env, className);
	LOGV("FindClass:%s, result=%d", className, clazz!=NULL);

	const char* methodName = symbolData[j]; //method 为分割得到的最后一个元素.
	const char* sign = GetMethodSign(env, className, methodName); //这个限制JNI接口的类不能有重载的接口函数.
	void* pFn = dlsym(handler, symbol);
	LOGV("dlsym:%s,methodName:%s,sign:%s, pFn=%d", symbol, methodName, sign,
			pFn!=NULL);

	if (methodName == NULL || sign == NULL || pFn == NULL)
		return;

	JNINativeMethod gMethods[] = { { methodName, sign, pFn } };

	int error = env->RegisterNatives(clazz, gMethods, 1);
	LOGV("RegisterNatives:%s, error= %d (0=success)", symbol, error);
}

