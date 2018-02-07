#include "ps4.h"
#include "kern.h"
#include "proc.h"
#include "gta.h"

extern char gtaPayload[];
extern int gtaPayloadSize;

int gamePID;
void* hookAddress;

double(*ceil)(double x);
int(*sceSysUtilSendSystemNotificationWithText)(int messageType, char* message);

void sysNotify(char* msg) {
	sceSysUtilSendSystemNotificationWithText(222, msg);
}

BOOL regionCheck() {
	procAttach(gamePID);

	u64 gameCheck;
	procReadBytes(gamePID, RegionCheckAddress, (void*)&gameCheck, sizeof(gameCheck));

	if (gameCheck == USRegionBytes) {
		sysNotify("US GTA V detected.");
		hookAddress = USHookAddress;
	}
	else if (gameCheck == EURegionBytes) {
		sysNotify("EU GTA V detected.");
		hookAddress = EUHookAddress;
	}
	else {
		sysNotify("Failed to detect GTA V.");
		procDetach(gamePID);
		return FALSE;
	}

	procDetach(gamePID);
	return TRUE;
}
BOOL setupDone() {
	procAttach(gamePID);

	BOOL allocationNeeded;
	procReadBytes(gamePID, &gtaVars->allocationNeeded, &allocationNeeded, sizeof(allocationNeeded));

	procDetach(gamePID);
	return !allocationNeeded;
}

void runSetup() {
	procAttach(gamePID);

	BOOL allocationNeeded = TRUE;
	procWriteBytes(gamePID, &gtaVars->allocationNeeded, &allocationNeeded, sizeof(allocationNeeded));

	void* null = NULL;
	procWriteBytes(gamePID, &gtaVars->executableSpace, &null, sizeof(null));
	procWriteBytes(gamePID, &gtaVars->dataSpace, &null, sizeof(null));

	int executableSize = (int)ceil((double)gtaPayloadSize / 0x4000) * 0x4000;
	procWriteBytes(gamePID, &gtaVars->allocationSize, &executableSize, sizeof(executableSize));

	procWriteBytes(gamePID, PayloadAddress, nativeHook, 0x1000);

	u8 syscallASM[] = { SyscallBytes };
	procWriteBytes(gamePID, SyscallAddress, syscallASM, sizeof(syscallASM));

	u8 hookASM[] = { HookBytes };
	procWriteBytes(gamePID, hookAddress, hookASM, sizeof(hookASM));

	procDetach(gamePID);
}
void startExecution() {
	procAttach(gamePID);

	void* executableSpace;
	procReadBytes(gamePID, &gtaVars->executableSpace, &executableSpace, sizeof(executableSpace));

	procWriteBytes(gamePID, executableSpace, gtaPayload, gtaPayloadSize);

	procDetach(gamePID);
}

int _main(void) {
	initKernel();
	initLibc();

	kexec(kernelPayload, NULL);

	int libc = sceKernelLoadStartModule("libSceLibcInternal.sprx", 0, NULL, 0, 0, 0);
	RESOLVE(libc, ceil);

	int sysUtil = sceKernelLoadStartModule("/system/common/lib/libSceSysUtil.sprx", 0, NULL, 0, 0, 0);
	RESOLVE(sysUtil, sceSysUtilSendSystemNotificationWithText);

	sysNotify("Payload loaded.\nPlease launch GTA V.");
	gamePID = findProcess("eboot.bin");

	sceKernelSleep(3);

	if (!regionCheck()) {
		return 0;
	}

	sysNotify("Setting up environment.");
	runSetup();

	while (!setupDone()) sceKernelSleep(3);

	startExecution();
	sysNotify("Mods activated.\nEnjoy!");

	return 0;
}