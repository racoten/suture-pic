#ifndef DEBUG_H
#define DEBUG_H

#include <windows.h>

#ifdef AGENT_DEBUG

void dbg(const char *title, const char *msg);
void dbg_hex(char *out, unsigned long long v);
void dbg_itoa(char *out, int val);
void dbg_ptr(const char *title, const char *label, void *ptr);
void dbg_int(const char *title, const char *label, int val);
void dbg_dump_addrs(const char *title, void *entries, int count);

#else /* !AGENT_DEBUG */

#define dbg(title, msg)                    ((void)0)
#define dbg_hex(out, v)                    ((void)0)
#define dbg_itoa(out, val)                 ((void)0)
#define dbg_ptr(title, label, ptr)         ((void)0)
#define dbg_int(title, label, val)         ((void)0)
#define dbg_dump_addrs(title, entries, n)  ((void)0)

#endif /* AGENT_DEBUG */

#endif /* DEBUG_H */
