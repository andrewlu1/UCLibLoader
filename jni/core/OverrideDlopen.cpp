/*
 * Copyright (C) 2009 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */
#include <assert.h>
#include <dlfcn.h>
#include <fcntl.h>
#include <pthread.h>
#include <setjmp.h>
#include <signal.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/ucontext.h>
#include <android/log.h>
#include <stdlib.h>
#include <stdio.h>

#ifndef __arm__
#error only for arm
#endif

#define LOGV(tag, ...)  (__android_log_print(ANDROID_LOG_INFO,tag,__VA_ARGS__))

struct DlGlobalData {
	struct sigaction oldaction;
	uintptr_t pos;
	pthread_key_t tls;
};

struct DlThreadData {
	jmp_buf where;
	void* returnVal;
	bool handling;
};

static struct DlGlobalData g_globaldata;

static struct DlThreadData* ensureGetThreadData() {
	struct DlThreadData* td =
			static_cast<struct DlThreadData*>(pthread_getspecific(
					g_globaldata.tls));
	if (td != NULL) {
		return td;
	}
	td = static_cast<struct DlThreadData*>(malloc(sizeof(struct DlThreadData)));
	pthread_setspecific(g_globaldata.tls, td);
	return td;
}

static void handleOldSigaction(int num, siginfo_t* i, void* w) {
	if (g_globaldata.oldaction.sa_handler == SIG_DFL ) {
		// die here
		_exit(1);
	}
	if (g_globaldata.oldaction.sa_handler == SIG_IGN ) {
		return;
	}
	g_globaldata.oldaction.sa_sigaction(num, i, w);
}

static void ucsigaction(int num, siginfo_t* i, void* w) {
	sigset_t set;
	struct DlThreadData* td =
			static_cast<struct DlThreadData*>(pthread_getspecific(
					g_globaldata.tls));
	if (td == NULL || !td->handling) {
		handleOldSigaction(num, i, w);
		return;
	}
	// fetch return value
	ucontext_t* crashContext = static_cast<ucontext_t*>(w);
	td->returnVal = reinterpret_cast<void*>(crashContext->uc_mcontext.arm_r0);
	sigemptyset(&set);
	sigaddset(&set, num);
	pthread_sigmask(SIG_UNBLOCK, &set, NULL);
	longjmp(td->where, 1);
}

void* uc_dlopen(const char* name, int flags) {
	typedef void* (*pfndlopen)(const char*, int);
	pfndlopen realdlopen = dlopen;
	struct DlThreadData* td = ensureGetThreadData();
	int posixReturnVal;
	struct sigaction ucsigactionval = { 0 };
	ucsigactionval.sa_sigaction = ucsigaction;
	ucsigactionval.sa_flags = SA_SIGINFO;

	posixReturnVal = sigaction(SIGSEGV, &ucsigactionval,
			&g_globaldata.oldaction);
	assert(posixReturnVal == 0);
	td->handling = true;
	if (!setjmp(td->where)) {
		asm volatile("\n"
				"mov r0, %0\n"
				"mov r1, %1\n"
				"mov ip, %2\n"
				"mov lr, %3\n"
				"bx ip\n"
				:
				: "r"(name), "r"(flags), "r"(realdlopen), "r"(g_globaldata.pos)
				: "r0", "r1", "ip", "lr"
		);
		//__builtin_unreachable();
		LOGV("JNI", "into uc_dlopen5");
	} else {
		posixReturnVal = sigaction(SIGSEGV, &g_globaldata.oldaction, NULL);
		assert(posixReturnVal == 0);
	}
	td->handling = false;

	return td->returnVal;
}

/**
 * findLibAddrV2();
 * 提高查找效率.
 */
static uintptr_t findLibAddrV2() {
	FILE* fd = fopen("/proc/self/maps", "r");
	if (fd == NULL)
		return 0ULL;
	LOGV("SoSymbol", "findLibAddrV2 start!");
	static int BUF_SIZE = 2047;
	char buffer[BUF_SIZE + 1];
	int readSize = 0;
	int alread_ReadSize = 0;
	while ((readSize = fread(buffer, sizeof(char), BUF_SIZE, fd)) != 0) {
		char* find = strstr(buffer, "/libc.so");
		if (!find) {
			alread_ReadSize += readSize;
			continue;
		} else {
			alread_ReadSize = alread_ReadSize + (find - buffer);
			break;
		}
	};
	if (alread_ReadSize > 100) {
		//找到/libc.so以后,向前倒退100字节. 这里假定libc这一行不会超过100. 经测试实际为70字节左右.
		fseek(fd, -100, SEEK_CUR);
		fread(buffer, sizeof(char), 100, fd);
		buffer[101] = '\0';//补加一个字符串结束符.
		char* lastEnd = strchr(buffer, '\n');
		if (lastEnd != NULL) {
			LOGV("SoSymbol", "findLibAddrV2 end!");
			unsigned long long addr = strtoull(lastEnd + 1, NULL, 16);
			return static_cast<uintptr_t>(addr);
		}
	}
	return 0ULL;
}

static void doinitGlobal() {
	memset(&g_globaldata, 0, sizeof(g_globaldata));
	pthread_key_create(&g_globaldata.tls, free);
	uintptr_t libcStart = findLibAddrV2();
	assert(libcStart != 0);
	g_globaldata.pos = libcStart;
}

static pthread_once_t once_control = PTHREAD_ONCE_INIT;

void uc_dlopenEnsureInit() {
	pthread_once(&once_control, doinitGlobal);
}

