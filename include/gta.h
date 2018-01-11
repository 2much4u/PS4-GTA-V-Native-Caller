#pragma once

#include "type.h"

// Sizes must be multiples of page size (0x4000)
#define ExecutableSpaceSize 0x10000
#define DataSpaceSize 0x4000

// Same for all regions
#define NativeTableAddress (void*)0x3644A28
#define PayloadAddress (void*)0x2B9D2B0
#define FreeSpaceAddress (void*)0x2DCD830
#define BaseVarsAddress (void*)0x2DCDA30

#define SyscallAddress (void*)0x2B9D28C
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

// Location not important, just any place in the EBOOT that the regions differ
#define RegionCheckAddress (void*)0xD8F304
#define USRegionBytes 0x8B10478B48535641
#define EURegionBytes 0x00841F0F2E666666

// IS_PLAYER_ONLINE
#define USHookAddress (void*)0xD8F489
#define EUHookAddress (void*)0xD8F499
#define SetupHookBytes 0x48, 0xC7, 0xC1, 0xB0, 0xD3, 0xB9, 0x02, 0xFF, 0xD1, 0x90
/*
mov rcx, 0x2B9D3B0 // PayloadAddress + 0x100
call rcx
nop
*/
#define ExecutionHookBytes 0x48, 0xC7, 0xC1, 0x30, 0xDA, 0xDC, 0x02, 0xFF, 0x11, 0x90
/*
mov rcx, 0x2DCDA30 // BaseVarsAddress
call [rcx]
nop
*/

enum Buttons {
	Button_Tpad = 0,
	Button_Tpad_X = 13,
	Button_Tpad_Y = 12,
	Button_X = 18,
	Button_Circle = 79,
	Button_Cirlce_2 = 56,
	Button_Triangle = 23,
	Button_Square = 22,
	Button_R1 = 44,
	Button_R2 = 11,
	Button_R3_X = 189,
	Button_R3_Y = 190,
	Button_R3_Right = 1,
	Button_R3_Down = 2,
	Button_R3_Left = 5,
	Button_R3_Up = 3,
	Button_R3_Push = 29,
	Button_L1 = 37,
	Button_L2 = 10,
	Button_L3_X = 187,
	Button_L3_Y = 188,
	Button_L3_Right = 9,
	Button_L3_Down = 8,
	Button_L3_Left = 34,
	Button_L3_Up = 32,
	Button_L3_Push = 36,
	Button_Dpad_Up = 27,
	Button_Dpad_Down = 19,
	Button_Dpad_Left = 15,
	Button_Dpad_Left_2 = 181,
	Button_Dpad_Right = 14,
	Button_Dpad_Right_2 = 182,
	Button_Options = 191
};

struct Native_s {
	struct Native_s *LastNativeTable;
	u64 NativeFunctions[7];
	u64 NativeCount;
	u64 NativeHashes[7];
};

struct NativeArg_s {
	u64* ReturnValue;
	short ArgCount;
	u64* ArgValues;
	short ReadCount;
};

#define gtaVars baseVars->_gtaVars
struct __gtaVars {
	BOOL init;
	int frameCount;
};

struct _baseVars {
	void* executableSpace;
	struct __gtaVars* _gtaVars;
	struct NativeArg_s nativeArg;
};
static struct _baseVars *baseVars = (struct _baseVars*)BaseVarsAddress;

void _invokeNative(u64 hash);
BOOL setupEnvironment();
BOOL nativeHook();