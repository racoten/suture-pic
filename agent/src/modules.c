#include <windows.h>
#include "agent.h"
#include "../libs/LibUtils/src/utils.h"

#define MAX_MODULES 64

static Module g_modules[MAX_MODULES];
static int    g_module_count = 0;

int module_register(AgentConfig *config, const Module *mod) {
    if (!mod || !mod->name || !mod->exec) return -1;
    if (g_module_count >= MAX_MODULES) return -2;

    g_modules[g_module_count] = *mod;
    g_module_count++;

    config->modules = g_modules;
    config->module_count = g_module_count;
    return 0;
}

Module *module_find(AgentConfig *config, const char *name) {
    (void)config;
    if (!name) return NULL;
    for (int i = 0; i < g_module_count; i++) {
        if (util_strcmp(g_modules[i].name, name) == 0)
            return &g_modules[i];
    }
    return NULL;
}

int module_dispatch(AgentConfig *config, const char *name,
                    const void *args, size_t args_len,
                    AgentOutput *output) {
    if (!output) return -1;

    output->type = OUTPUT_TYPE_EMPTY;
    output->data = NULL;
    output->data_len = 0;
    output->error_code = 0;

    Module *mod = module_find(config, name);
    if (!mod) {
        output->type = OUTPUT_TYPE_ERROR;
        output->error_code = -1;
        return -1;
    }

    return mod->exec(config, args, args_len, output);
}

void output_free(AgentOutput *output) {
    if (output && output->data) {
        util_free(output->data);
        output->data = NULL;
        output->data_len = 0;
    }
}
