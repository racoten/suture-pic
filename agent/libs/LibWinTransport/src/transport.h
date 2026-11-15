#ifndef TRANSPORT_H
#define TRANSPORT_H

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

int   transport_startup(void);
void *transport_connect(const char *host, const char *port);
int   transport_send(void *connection, const void *data, size_t length);
int   transport_receive(void *connection, void *buffer, size_t capacity);
void  transport_close(void *connection);
void  transport_shutdown(void);

#ifdef __cplusplus
}
#endif

#endif
