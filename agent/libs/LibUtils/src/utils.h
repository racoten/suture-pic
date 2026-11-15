#ifndef UTILS_H
#define UTILS_H

#include <windows.h>
#include <stddef.h>

void *util_malloc(size_t size);
void  util_free(void *ptr);
void *util_realloc(void *ptr, size_t size);
void  util_memset(void *dst, int val, size_t len);
void  util_memcpy(void *dst, const void *src, size_t len);
int   util_memcmp(const void *a, const void *b, size_t len);

int    util_strlen(const char *s);
int    util_strcmp(const char *a, const char *b);
int    util_strncmp(const char *a, const char *b, int n);
void   util_strcpy(char *dst, const char *src);
void   util_strncpy(char *dst, const char *src, int n);
char  *util_strcat(char *dst, const char *src);
char  *util_strchr(const char *s, int c);

int    util_wcslen(const WCHAR *s);

void   util_itoa(char *out, int val);
void   util_utoa(char *out, unsigned int val);
void   util_hex(char *out, unsigned long long val);
int    util_atoi(const char *s);

LPWSTR util_utf8_to_wide(const char *utf8);
char  *util_wide_to_utf8(const WCHAR *wide);

#endif
