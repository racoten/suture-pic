#ifndef RECON_H
#define RECON_H

#include <windows.h>
#include <stddef.h>

int recon_ifconfig(void **out_data, size_t *out_len);
int recon_arp(void **out_data, size_t *out_len);
int recon_netstat(void **out_data, size_t *out_len);
int recon_route(void **out_data, size_t *out_len);
int recon_portscan(const char *target, int start_port, int end_port, void **out_data, size_t *out_len);
int recon_dns_resolve(const char *hostname, char *buf, size_t capacity);

int recon_domain_info(void **out_data, size_t *out_len);
int recon_domain_users(void **out_data, size_t *out_len);
int recon_domain_groups(void **out_data, size_t *out_len);
int recon_domain_computers(void **out_data, size_t *out_len);

int recon_net_shares(const char *target, void **out_data, size_t *out_len);
int recon_net_sessions(const char *target, void **out_data, size_t *out_len);

int recon_sysinfo(void **out_data, size_t *out_len);
int recon_local_users(void **out_data, size_t *out_len);
int recon_local_groups(void **out_data, size_t *out_len);
int recon_services(void **out_data, size_t *out_len);
int recon_installed_software(void **out_data, size_t *out_len);

#endif
