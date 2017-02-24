/*2much4u*/

#include "gta.h"

void invokeNative_s(u64 hash) {
    struct Native_s** g_Natives = (struct Native_s**)NativeTableAddress;
    struct Native_s* Natives = g_Natives[hash & 0xFF];
    while (Natives != 0) {
        for (unsigned int i = 0; i < Natives->NativeCount; i++) {
            if (Natives->NativeHashes[i] == hash) {
                ((void(*)(struct NativeArg_s*))Natives->NativeFunctions[i])((struct NativeArg_s*)FreeSpaceAddress);
                return;
            }
        }
        Natives = Natives->LastNativeTable;
    }
}

struct gtaVars_s {
    BOOL init;
    int frameCount;
};

static struct gtaVars_s *gtaVars = (struct gtaVars_s*)FreeSpaceAddress+0x100;

BOOL nativeHook() {
    if (!gtaVars->init) {
        nativeArg->ArgValues = args;
        nativeArg->ReturnValue = rets;
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

        if (IS_CONTROL_JUST_PRESSED(0, Button_Tpad)) {
            Entity teleportEntity = pedID;
            if (IS_PED_IN_ANY_VEHICLE(pedID, FALSE)) {
                teleportEntity = GET_VEHICLE_PED_IS_USING(pedID);
            }

            SET_ENTITY_COORDS(teleportEntity, -74.94243f, -818.63446f, 326.174347f, FALSE, FALSE, FALSE, TRUE);
        }
    }

    return TRUE;
}
