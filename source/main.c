#include "ps4.h"
#include "kern.h"
#include "proc.h"
#include "gta.h"

int gamePID;
void* hookAddress;

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

	void* executionSpace;
	void* dataSpace;
	procReadBytes(gamePID, baseVars, &executionSpace, sizeof(executionSpace));
	procReadBytes(gamePID, &gtaVars, &dataSpace, sizeof(dataSpace));

	procDetach(gamePID);
	return (executionSpace != 0) && (dataSpace != 0);
}

void runSetup() {
	procAttach(gamePID);

	procWriteBytes(gamePID, PayloadAddress, _invokeNative, 0x100);
	procWriteBytes(gamePID, PayloadAddress + 0x100, setupEnvironment, 0x1000);

	u8 syscallASM[] = { SyscallBytes };
	procWriteBytes(gamePID, SyscallAddress, syscallASM, sizeof(syscallASM));

	u8 hookASM[] = { SetupHookBytes };
	procWriteBytes(gamePID, hookAddress, hookASM, sizeof(hookASM));

	procDetach(gamePID);
}
void startExecution() {
	procAttach(gamePID);

	void* executableSpace;
	procReadBytes(gamePID, baseVars, &executableSpace, sizeof(executableSpace));
	
	procWriteBytes(gamePID, executableSpace, nativeHook, ExecutableSpaceSize);

	u8 hookASM[] = { ExecutionHookBytes };
	procWriteBytes(gamePID, hookAddress, hookASM, sizeof(hookASM));

	procDetach(gamePID);
}

int _main(void) {
	initKernel();
	initLibc();
	initNetwork();
		
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

	if (setupDone()) startExecution();
	else {
		debugPrint("Setting up environment...\n");
		runSetup();

		while (!setupDone()) sceKernelSleep(3);

		startExecution();
	}
	debugPrint("Process patched\n");

	debugPrint("Finished payload");
	disableDebug();

	return 0;
}