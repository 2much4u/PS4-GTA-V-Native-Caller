#pragma once

#include "types.h"
#include <utility>

struct NativeArg_s {
	u64* returnValue;
	u32 argCount;
	u8 padding1[4];
	u64* argValues;
	u32 vectorCount;
	u8 padding2[4];
	Vector3* argVectors[4];
	Vector4 tempVectors[4];
};

extern NativeArg_s nativeArg;

extern void resetArgs();
extern void setVectors();

template<typename T>
inline void pushArg(T value) {
	*(T*)&nativeArg.argValues[nativeArg.argCount] = value;
	nativeArg.argCount++;
}

template<typename R>
inline R getReturn() {
	return *(R*)&nativeArg.returnValue[0];
}

template<typename N, typename... A>
N invoke(u64 nativeAddress, A &&... args)
{
	resetArgs();
	int dummy[] = { 0, ((void)pushArg(std::forward<A>(args)), 0) ... };
	((void(*)(NativeArg_s*))(void*)nativeAddress)(&nativeArg);
	setVectors();
	return getReturn<N>();
}