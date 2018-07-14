#include "ps4.h"
#include "kern.h"
#include "proc.h"
#include "gta.h"

extern char gtaPayload[];
extern int gtaPayloadSize;

int gamePID;

double(*ceil)(double x);
int(*sceSysUtilSendSystemNotificationWithText)(int messageType, char* message);

void sysNotify(char* msg) {
	sceSysUtilSendSystemNotificationWithText(222, msg);
}

BOOL gameCheck() {
	procAttach(gamePID);

	u64 testBytes;
	procReadBytes(gamePID, GameCheckAddress, (void*)&testBytes, sizeof(testBytes));

	BOOL success = FALSE;
	switch (testBytes)
	{
	case GameCheck127Bytes:
		sysNotify("1.27 GTA V detected.");
		success = TRUE;
		break;
	case GameCheck100USBytes:
		sysNotify("1.00 US GTA V detected.\nThis payload can only be used on 1.27 GTA V.");
		break;
	case GameCheck100EUBytes:
		sysNotify("1.00 EU GTA V detected.\nThis payload can only be used on 1.27 GTA V.");
		break;
	default:
		sysNotify("Failed to detect GTA V.");
		break;
	}

	procDetach(gamePID);
	return success;
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
	procWriteBytes(gamePID, HookAddress, hookASM, sizeof(hookASM));

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

	if (!gameCheck()) {
		return 0;
	}

	sysNotify("Setting up environment.");
	runSetup();

	while (!setupDone()) sceKernelSleep(3);

	startExecution();
	sysNotify("Mods activated.\nEnjoy!");

	return 0;
}