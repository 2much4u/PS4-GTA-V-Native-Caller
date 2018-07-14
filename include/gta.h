#pragma once

#include "type.h"

// Same for all regions
#define PayloadAddress (void*)0x31B61A4
#define GTAVarsAddress (void*)0x31C80FA

#define SyscallAddress (void*)0x31B616C
#define SyscallBytes 0x48, 0x89, 0xF8, 0x48, 0x89, 0xF7, 0x48, 0x89, 0xD6, 0x48, 0x89, 0xCA, 0x4D, 0x89, 0xC2, 0x4D, 0x89, 0xC8, 0x4C, 0x8B, 0x4C, 0x24, 0x08, 0x0F, 0x05, 0xC3
/*
mov rax, rdi
mov rdi, rsi
mov rsi, rdx
mov rdx, rcx
mov r10, r8
mov r8, r9
mov r9, [rsp+0x8]
syscall
ret
*/

// Location not important, this is a random place in the EBOOT to detect region and version
#define GameCheckAddress (void*)0xD8F304
#define GameCheck127Bytes 0xD089000000A78388
#define GameCheck100USBytes 0x8B10478B48535641
#define GameCheck100EUBytes 0x00841F0F2E666666

// IS_PLAYER_ONLINE
#define HookAddress (void*)0xFBDD59
#define HookBytes 0x48, 0xC7, 0xC1, 0xA4, 0x61, 0x1B, 0x03, 0xFF, 0xD1, 0x90
/*
mov rcx, 0x31B61A4 // PayloadAddress
call rcx
nop
*/

struct _gtaVars {
	BOOL allocationNeeded;
	int allocationSize;
	void* executableSpace;
	void* dataSpace;
};
static struct _gtaVars *gtaVars = (struct _gtaVars*)GTAVarsAddress;

extern BOOL nativeHook();