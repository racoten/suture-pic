#define WIN32_LEAN_AND_MEAN
#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>
#include <limits.h>
#include "transport.h"
#include "debug.h"

WINSOCK_API_LINKAGE int WINAPI WS2_32$WSAStartup(WORD, LPWSADATA);
WINSOCK_API_LINKAGE int WINAPI WS2_32$WSACleanup(void);
WINSOCK_API_LINKAGE INT WINAPI WS2_32$getaddrinfo(PCSTR, PCSTR, const ADDRINFOA *, PADDRINFOA *);
WINSOCK_API_LINKAGE void WINAPI WS2_32$freeaddrinfo(PADDRINFOA);
WINSOCK_API_LINKAGE SOCKET WINAPI WS2_32$socket(int, int, int);
WINSOCK_API_LINKAGE int WINAPI WS2_32$connect(SOCKET, const struct sockaddr *, int);
WINSOCK_API_LINKAGE int WINAPI WS2_32$closesocket(SOCKET);
WINSOCK_API_LINKAGE int WINAPI WS2_32$send(SOCKET, const char *, int, int);
WINSOCK_API_LINKAGE int WINAPI WS2_32$recv(SOCKET, char *, int, int);
WINSOCK_API_LINKAGE int WINAPI WS2_32$WSAGetLastError(void);

int transport_startup(void)
{
    WSADATA data;
    return WS2_32$WSAStartup(MAKEWORD(2, 2), &data) == 0 ? 0 : -1;
}

void *transport_connect(const char *host, const char *port)
{
    struct addrinfo hints;
    struct addrinfo *addresses = NULL;
    struct addrinfo *address;
    SOCKET connected = INVALID_SOCKET;

    if (host == NULL || port == NULL) return NULL;

    { char *z = (char *)&hints; for (int i = 0; i < (int)sizeof(hints); i++) z[i] = 0; }
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    if (WS2_32$getaddrinfo(host, port, &hints, &addresses) != 0)
        return NULL;

    for (address = addresses; address != NULL; address = address->ai_next) {
        SOCKET candidate = WS2_32$socket(
            address->ai_family,
            address->ai_socktype,
            address->ai_protocol
        );
        if (candidate == INVALID_SOCKET) continue;

        if (WS2_32$connect(candidate, address->ai_addr, (int)address->ai_addrlen) == 0) {
            connected = candidate;
            break;
        }
        WS2_32$closesocket(candidate);
    }

    WS2_32$freeaddrinfo(addresses);

    if (connected == INVALID_SOCKET) return NULL;
    return (void *)connected;
}

int transport_send(void *connection, const void *data, size_t length)
{
    SOCKET socket_handle = (SOCKET)connection;
    const char *bytes = (const char *)data;
    size_t sent_total = 0;

    if (connection == NULL || (data == NULL && length != 0)) return -1;

    while (sent_total < length) {
        size_t remaining = length - sent_total;
        int chunk_size = remaining > (size_t)INT_MAX ? INT_MAX : (int)remaining;

        int sent = WS2_32$send(socket_handle, bytes + sent_total, chunk_size, 0);
        if (sent == SOCKET_ERROR || sent == 0) return -1;

        sent_total += (size_t)sent;
    }
    return 0;
}

int transport_receive(void *connection, void *buffer, size_t capacity)
{
    SOCKET socket_handle = (SOCKET)connection;
    if (connection == NULL || buffer == NULL || capacity == 0) return -1;

    int chunk_size = capacity > (size_t)INT_MAX ? INT_MAX : (int)capacity;
    return WS2_32$recv(socket_handle, (char *)buffer, chunk_size, 0);
}

void transport_close(void *connection)
{
    if (connection != NULL)
        WS2_32$closesocket((SOCKET)connection);
}

void transport_shutdown(void)
{
    WS2_32$WSACleanup();
}
