#include "gta.h"
#include "ps4.h"

static u64 (*_syscall)(int num, ...) = (void*)SyscallAddress;
Inline void* sys_mmap(void *addr, size_t len, int prot, int flags, int fd, off_t offset) {
	return (void*)_syscall(477, addr, len, prot, flags, fd, offset);
}

BOOL nativeHook() {
	if (gtaVars->allocationNeeded) {
		if (!gtaVars->executableSpace) {
			gtaVars->executableSpace = sys_mmap((void*)0x926200000, gtaVars->allocationSize, PROT_READ | PROT_WRITE | PROT_EXEC, MAP_PRIVATE | MAP_ANONYMOUS | MAP_FIXED, -1, 0);
		}
		else if (!gtaVars->dataSpace) {
			gtaVars->dataSpace = sys_mmap((void*)0x926300000, gtaVars->allocationSize, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS | MAP_FIXED, -1, 0);
		}
		else {
			*(char*)gtaVars->executableSpace = 0xC3; // retn
			gtaVars->allocationNeeded = FALSE;
		}
	}
	else {
		((void(*)())gtaVars->executableSpace)();
	}

	return TRUE;
}