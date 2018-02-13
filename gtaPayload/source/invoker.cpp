#include "invoker.h"

#define NativeTableAddress 0x3644A28

NativeArg_s nativeArg;
u64 args[30];

void setVectors() {
	while (nativeArg.vectorCount) {
		nativeArg.vectorCount--;

		Vector3* argVector = nativeArg.argVectors[nativeArg.vectorCount];
		Vector4 tempVector = nativeArg.tempVectors[nativeArg.vectorCount];
		argVector->x = tempVector.x;
		argVector->y = tempVector.y;
		argVector->z = tempVector.z;
	}
}

void callHash(u64 hash) {
	Native_s **nativeTable = (Native_s **)NativeTableAddress;
	Native_s *natives = nativeTable[hash & 0xFF];
	while (natives != 0) {
		for (u64 i = 0; i < natives->nativeCount; i++) {
			if (natives->nativeHashes[i] == hash) {
				((void(*)(NativeArg_s*))natives->nativeFunctions[i])(&nativeArg);
				return;
			}
		}
		natives = natives->lastNativeTable;
	}
}

void resetArgs() {
	nativeArg.argCount = 0;
	nativeArg.vectorCount = 0;
	nativeArg.argValues = nativeArg.returnValue = args;
}