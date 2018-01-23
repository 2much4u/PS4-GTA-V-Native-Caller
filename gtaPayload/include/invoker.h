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
	short argCount;
	u64* argValues;
	short readCount;
};

extern NativeArg_s nativeArg;

void callHash(u64 hash);
void resetArgs();

template<typename T>
inline void pushArg(T value)
{
	*(T*)&nativeArg.argValues[nativeArg.argCount] = value;
	nativeArg.argCount++;
}

template<typename R>
inline R getReturn()
{
	R* result = (R*)&nativeArg.returnValue[0];
	return *(R*)result;
}

template<typename N, typename... A>
N invoke(u64 hash, A &&... args)
{
	resetArgs();
	int dummy[] = { 0, ((void)pushArg(std::forward<A>(args)), 0) ... };
	callHash(hash);
	return getReturn<N>();
}