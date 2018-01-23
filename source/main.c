#include "ps4.h"
#include "kern.h"
#include "proc.h"
#include "gta.h"

extern char gtaPayload[];
extern int gtaPayloadSize;

int gamePID;
void* hookAddress;

double(*ceil)(double x);

#undef DEBUG
#ifdef DEBUG
int debugSock;

#define debugPrint(format, ...)\
		do {\
			char buffer[512];\
			int size = sprintf(buffer, format, ##__VA_ARGS__);\
			sceNetSend(debugSock, buffer, size, 0);\
		} while(0)\

void enableDebug() {
	char debugSocketName[] = "debug";
	struct sockaddr_in debugServer;
	debugServer.sin_len = sizeof(debugServer);
	debugServer.sin_family = AF_INET;
	debugServer.sin_addr.s_addr = IP(10, 0, 0, 60);
	debugServer.sin_port = sceNetHtons(9024);
	memset(debugServer.sin_zero, 0, sizeof(debugServer.sin_zero));
	debugSock = sceNetSocket(debugSocketName, AF_INET, SOCK_STREAM, 0);
	sceNetConnect(debugSock, (struct sockaddr *)&debugServer, sizeof(debugServer));
}

void disableDebug() {
	sceNetSocketClose(debugSock);
}
#else
#define debugPrint(...)
#define enableDebug()
#define disableDebug()
#endif

BOOL regionCheck() {
	procAttach(gamePID);

	u64 gameCheck;
	procReadBytes(gamePID, RegionCheckAddress, (void*)&gameCheck, sizeof(gameCheck));

	if (gameCheck == USRegionBytes) {
		debugPrint("US GTA V detected\n");
		hookAddress = USHookAddress;
	}
	else if (gameCheck == EURegionBytes) {
		debugPrint("EU GTA V detected\n");
		hookAddress = EUHookAddress;
	}
	else {
		debugPrint("Failed to detect GTA V\n");
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
	debugPrint("Allocating 2 * 0x%llX bytes within GTA...\n", executableSize);

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
	initNetwork();

	int libc = sceKernelLoadStartModule("libSceLibcInternal.sprx", 0, NULL, 0, 0, 0);
	RESOLVE(libc, ceil);

	enableDebug();
	debugPrint("Started payload\n");

	kexec(kernelPayload, NULL);
	debugPrint("Kernel patched\n");

	debugPrint("Searching for EBOOT...\n");
	gamePID = findProcess("eboot.bin");
	debugPrint("Found EBOOT at PID: %i\n", gamePID);

	sceKernelSleep(3);

	if (!regionCheck()) {
		debugPrint("Finished payload");
		disableDebug();

		return 0;
	}

	debugPrint("Setting up environment...\n");
	runSetup();

	while (!setupDone()) sceKernelSleep(3);

	startExecution();
	debugPrint("Process patched\n");

	debugPrint("Finished payload");
	disableDebug();

	return 0;
}