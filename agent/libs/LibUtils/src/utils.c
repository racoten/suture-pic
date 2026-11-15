#include <windows.h>
#include "utils.h"

WINBASEAPI void * MSVCRT$malloc(SIZE_T);
WINBASEAPI void   MSVCRT$free(void *);
WINBASEAPI void * MSVCRT$realloc(void *, size_t);

WINBASEAPI int WINAPI KERNELBASE$MultiByteToWideChar(
    UINT, DWORD, LPCCH, int, LPWSTR, int);
WINBASEAPI int WINAPI KERNELBASE$WideCharToMultiByte(
    UINT, DWORD, LPCWCH, int, LPSTR, int, LPCCH, LPBOOL);

void *util_malloc(size_t size) {
    return MSVCRT$malloc(size);
}

void util_free(void *ptr) {
    if (ptr) MSVCRT$free(ptr);
}

void *util_realloc(void *ptr, size_t size) {
    return MSVCRT$realloc(ptr, size);
}

void util_memset(void *dst, int val, size_t len) {
    unsigned char *p = (unsigned char *)dst;
    while (len--) *p++ = (unsigned char)val;
}

void util_memcpy(void *dst, const void *src, size_t len) {
    unsigned char *d = (unsigned char *)dst;
    const unsigned char *s = (const unsigned char *)src;
    while (len--) *d++ = *s++;
}

int util_memcmp(const void *a, const void *b, size_t len) {
    const unsigned char *pa = (const unsigned char *)a;
    const unsigned char *pb = (const unsigned char *)b;
    while (len--) {
        if (*pa != *pb) return *pa - *pb;
        pa++; pb++;
    }
    return 0;
}

int util_strlen(const char *s) {
    int n = 0;
    if (!s) return 0;
    while (s[n]) n++;
    return n;
}

int util_strcmp(const char *a, const char *b) {
    while (*a && *b && *a == *b) { a++; b++; }
    return (unsigned char)*a - (unsigned char)*b;
}

int util_strncmp(const char *a, const char *b, int n) {
    while (n-- > 0) {
        if (*a != *b || !*a) return (unsigned char)*a - (unsigned char)*b;
        a++; b++;
    }
    return 0;
}

void util_strcpy(char *dst, const char *src) {
    while ((*dst++ = *src++));
}

void util_strncpy(char *dst, const char *src, int n) {
    while (n-- > 0 && (*dst++ = *src++));
    if (n >= 0) *dst = '\0';
}

char *util_strcat(char *dst, const char *src) {
    char *p = dst;
    while (*p) p++;
    while ((*p++ = *src++));
    return dst;
}

char *util_strchr(const char *s, int c) {
    while (*s) {
        if (*s == (char)c) return (char *)s;
        s++;
    }
    return (c == '\0') ? (char *)s : NULL;
}

int util_wcslen(const WCHAR *s) {
    int n = 0;
    if (!s) return 0;
    while (s[n]) n++;
    return n;
}

void util_itoa(char *out, int val) {
    char *p = out;
    if (val < 0) { *p++ = '-'; val = -val; }
    if (val == 0) { *p++ = '0'; *p = '\0'; return; }
    int d = 1000000000;
    while (d > val) d /= 10;
    while (d > 0) { *p++ = '0' + (val / d); val %= d; d /= 10; }
    *p = '\0';
}

void util_utoa(char *out, unsigned int val) {
    char *p = out;
    if (val == 0) { *p++ = '0'; *p = '\0'; return; }
    unsigned int d = 1000000000;
    while (d > val) d /= 10;
    while (d > 0) { *p++ = '0' + (val / d); val %= d; d /= 10; }
    *p = '\0';
}

void util_hex(char *out, unsigned long long v) {
    const char *h = "0123456789abcdef";
    out[0] = '0'; out[1] = 'x';
    for (int i = 15; i >= 0; i--)
        out[2 + (15 - i)] = h[(v >> (i * 4)) & 0xF];
    out[18] = '\0';
}

int util_atoi(const char *s) {
    int val = 0, neg = 0;
    if (!s) return 0;
    if (*s == '-') { neg = 1; s++; }
    while (*s >= '0' && *s <= '9') {
        val = val * 10 + (*s - '0');
        s++;
    }
    return neg ? -val : val;
}

LPWSTR util_utf8_to_wide(const char *utf8) {
    if (!utf8) return NULL;
    int len = KERNELBASE$MultiByteToWideChar(CP_UTF8, 0, utf8, -1, NULL, 0);
    if (len == 0) return NULL;
    LPWSTR wide = (LPWSTR)MSVCRT$malloc(len * sizeof(WCHAR));
    if (!wide) return NULL;
    if (KERNELBASE$MultiByteToWideChar(CP_UTF8, 0, utf8, -1, wide, len) == 0) {
        MSVCRT$free(wide);
        return NULL;
    }
    return wide;
}

char *util_wide_to_utf8(const WCHAR *wide) {
    if (!wide) return NULL;
    int len = KERNELBASE$WideCharToMultiByte(CP_UTF8, 0, wide, -1, NULL, 0, NULL, NULL);
    if (len == 0) return NULL;
    char *utf8 = (char *)MSVCRT$malloc(len);
    if (!utf8) return NULL;
    if (KERNELBASE$WideCharToMultiByte(CP_UTF8, 0, wide, -1, utf8, len, NULL, NULL) == 0) {
        MSVCRT$free(utf8);
        return NULL;
    }
    return utf8;
}
