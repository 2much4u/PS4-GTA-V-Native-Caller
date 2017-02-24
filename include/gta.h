/*2much4u*/

#pragma once

#include "types.h"

#define TRUE 1
#define FALSE 0
#define Inline inline __attribute__((always_inline))

typedef int Any;
typedef unsigned int uint;
typedef int Hash;
typedef int BOOL;
typedef int Entity;
typedef int Player;
typedef int FireId;
typedef int Ped;
typedef int Vehicle;
typedef int Cam;
typedef int CarGenerator;
typedef int Group;
typedef int Train;
typedef int Pickup;
typedef int Object;
typedef int Weapon;
typedef int Interior;
typedef int Blip;
typedef int Texture;
typedef int TextureDict;
typedef int CoverPoint;
typedef int Camera;
typedef int TaskSequence;
typedef int ColourIndex;
typedef int Sphere;
typedef int ScrHandle;

typedef uint64_t u64;
typedef uint32_t u32;
typedef uint8_t u8;


struct Vector3 {
	float x, y, z;
};

struct color {
    u8 r, g, b;
};

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
	Button_R3_Right = 1,
	Button_R3_Down = 2,
	Button_R3_Left =5,
	Button_R3_Up = 3,
	Button_R3_Push = 29,
	Button_L1 = 37,
	Button_L2 = 10,
	Button_L3_Right = 9,
	Button_L3_Down = 8,
	Button_L3_Left =34,
	Button_L3_Up = 32,
	Button_L3_Push = 36,
	Button_Dpad_Up = 27,
	Button_Dpad_Down = 19,
	Button_Dpad_Left = 15,
	Button_Dpad_Right = 14,
	Button_Options = 191
};

//Same for all regions
#define NativeTableAddress 0x3644A28
#define PayloadAddress 0x2B9D2A0
#define FreeSpaceAddress 0x2DCD830

//Location not important, just any place in the EBOOT that the regions differ
#define RegionCheckAddress 0xD8F304
#define USRegionBytes 0x8B10478B48535641
#define EURegionBytes 0x00841F0F2E666666

//IS_PLAYER_ONLINE
#define USHookAddress 0xD8F489
#define EUHookAddress 0xD8F499
#define HookBytes 0x48, 0xC7, 0xC1, 0xA0, 0xD3, 0xB9, 0x02, 0xFF, 0xD1, 0x90
/**
mov rcx, 0x2B9D3A0  //PayloadAddress+0x100
call rcx
nop
**/

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

static void (*invokeNative)(u64 hash) = (void*)PayloadAddress;
static struct NativeArg_s *nativeArg = (struct NativeArg_s*)FreeSpaceAddress;
static u64 *args = (u64*)FreeSpaceAddress+0x20;
static u64 *rets = (u64*)FreeSpaceAddress+0x20;

#include "natives.h"

void invokeNative_s(u64 hash);
BOOL nativeHook();