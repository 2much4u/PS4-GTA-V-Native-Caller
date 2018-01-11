#include "gta.h"
#include "type.h"
#include "natives.h"
#include "ps4.h"

void _invokeNative(u64 hash) {
	struct Native_s** g_Natives = (struct Native_s**)NativeTableAddress;
	struct Native_s* Natives = g_Natives[hash & 0xFF];
	while (Natives != 0) {
		for (unsigned int i = 0; i < Natives->NativeCount; i++) {
			if (Natives->NativeHashes[i] == hash) {
				((void(*)(struct NativeArg_s*))Natives->NativeFunctions[i])(&baseVars->nativeArg);
				return;
			}
		}
		Natives = Natives->LastNativeTable;
	}
}

static u64 (*_syscall)(int num, ...) = (void*)SyscallAddress;
Inline void* sys_mmap(void *addr, size_t len, int prot, int flags, int fd, off_t offset) {
	return (void*)_syscall(477, addr, len, prot, flags, fd, offset);
}

BOOL setupEnvironment() {
	if (!baseVars->executableSpace) {
		baseVars->executableSpace = sys_mmap(NULL, ExecutableSpaceSize, PROT_READ | PROT_WRITE | PROT_EXEC, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
	}
	if (!gtaVars) {
		gtaVars = sys_mmap(NULL, DataSpaceSize, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
	}

	return TRUE;
}

BOOL nativeHook() {
	if (!gtaVars->init) {
		baseVars->nativeArg.ArgValues = args;
		baseVars->nativeArg.ReturnValue = rets;
		gtaVars->frameCount = 0;
		gtaVars->init = TRUE;
	}

	int newFrameCount = GET_FRAME_COUNT();
	if (gtaVars->frameCount < newFrameCount) {
		gtaVars->frameCount = newFrameCount;

		Ped pedID = PLAYER_PED_ID();
		Player playerID = PLAYER_ID();

		SET_SUPER_JUMP_THIS_FRAME(playerID);
		SET_PLAYER_INVINCIBLE(playerID, TRUE);

		if (IS_CONTROL_JUST_PRESSED(0, Button_Square) && IS_CONTROL_JUST_PRESSED(0, Button_Dpad_Right)) {
			Entity teleportEntity = pedID;
			if (IS_PED_IN_ANY_VEHICLE(pedID, FALSE)) {
				teleportEntity = GET_VEHICLE_PED_IS_USING(pedID);
			}

			SET_ENTITY_COORDS(teleportEntity, -74.94243f, -818.63446f, 326.174347f, FALSE, FALSE, FALSE, TRUE); // Top of Maze Bank
		}
	}

	return TRUE;
}