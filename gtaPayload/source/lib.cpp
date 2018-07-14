#include "lib.h"

u64 (*syscall)(int num, ...) = (void*)0x31B616C;
#define resolve(module, name) syscall(591, module, #name, &name);

void *(*malloc)(size_t size);
void(*free)(void *ptr);
void *(*calloc)(size_t num, size_t size);
void *(*realloc)(void* ptr, size_t size);
void *(*memset)(void *destination, int value, size_t num);
void *(*memcpy)(void *destination, const void *source, size_t num);
int(*memcmp)(const void *s1, const void *s2, size_t n);
char *(*strcpy)(char *destination, const char *source);
char *(*strncpy)(char *destination, const char *source, size_t num);
char *(*strcat)(char *dest, const char *src);
char *(*strncat)(char *dest, const char *src, size_t n);
size_t(*strlen)(const char *s);
int(*strcmp)(const char *s1, const char *s2);
int(*strncmp)(const char *s1, const char *s2, size_t n);
int(*sprintf)(char *str, const char *format, ...);
int(*snprintf)(char *str, size_t size, const char *format, ...);
int(*sscanf)(const char *str, const char *format, ...);
char *(*strchr)(const char *s, int c);
char *(*strrchr)(const char *s, int c);
char *(*strstr)(char *str1, char *str2);
int(*atoi)(const char * str);
double(*atof)(const char * str);

void resolveLibs() {
	int libcHandle;
	syscall(594, "libSceLibcInternal.sprx", 0, &libcHandle, 0);

	resolve(libcHandle, malloc);
	resolve(libcHandle, free);
	resolve(libcHandle, calloc);
	resolve(libcHandle, realloc);
	resolve(libcHandle, memset);
	resolve(libcHandle, memcpy);
	resolve(libcHandle, memcmp);
	resolve(libcHandle, strcpy);
	resolve(libcHandle, strncpy);
	resolve(libcHandle, strcat);
	resolve(libcHandle, strncat);
	resolve(libcHandle, strlen);
	resolve(libcHandle, strcmp);
	resolve(libcHandle, strncmp);
	resolve(libcHandle, sprintf);
	resolve(libcHandle, snprintf);
	resolve(libcHandle, sscanf);
	resolve(libcHandle, strchr);
	resolve(libcHandle, strrchr);
	resolve(libcHandle, strstr);
	resolve(libcHandle, atoi);
	resolve(libcHandle, atof);
}

void initLibs() {
	do {
		resolveLibs();
	} while (malloc == nullptr);
}