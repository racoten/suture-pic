#ifndef LATERAL_H
#define LATERAL_H

#include <windows.h>
#include <stddef.h>

int lateral_wmi_exec(const char *target, const char *command);
int lateral_scm_exec(const char *target, const char *bin_path);
int lateral_schtask_exec(const char *target, const char *command);

int lateral_smb_upload(const char *target, const char *share, const char *remote_path, const void *data, size_t len);
int lateral_smb_download(const char *target, const char *share, const char *remote_path, void **out_data, size_t *out_len);

int lateral_jump_exec(const char *target, const void *shellcode, size_t sc_len);

#endif
