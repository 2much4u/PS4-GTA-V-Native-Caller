#pragma once

#include "types.h"
#include <utility>

struct Native_s {
	struct Native_s *lastNativeTable;
	u64 nativeFunctions[7];
	u64 nativeCount;
	u64 nativeHashes[7];
};

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

void callHash(u64 hash);
void resetArgs();
void setVectors();

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
N invoke(u64 hash, A &&... args)
{
	resetArgs();
	int dummy[] = { 0, ((void)pushArg(std::forward<A>(args)), 0) ... };
	callHash(hash);
	setVectors();
	return getReturn<N>();
}