#include <windows.h>
#include "../agent/libs/LibCPLTest/src/cpltest.h"
#include "../agent/libs/LibUtils/src/utils.h"
#include "../agent/src/agent.h"

static int mock_exec_called = 0;
static int mock_exec_result = 0;

static int mock_exec(AgentConfig *config, const void *args,
                     size_t args_len, AgentOutput *output) {
    (void)config;
    (void)args;
    (void)args_len;
    mock_exec_called = 1;
    output->type = OUTPUT_TYPE_TEXT;
    output->data = NULL;
    output->data_len = 0;
    output->error_code = 0;
    return mock_exec_result;
}

void test_module_register(void) {
    AgentConfig config;
    util_memset(&config, 0, sizeof(config));
    config.modules = NULL;
    config.module_count = 0;

    Module mod;
    util_memset(&mod, 0, sizeof(mod));
    mod.name = "mock-cmd";
    mod.category = "test";
    mod.description = "A mock module for testing";
    mod.exec = mock_exec;
    mod.opsec_safe = TRUE;

    int rc = module_register(&config, &mod);
    ASSERT(rc == 0, "module_register: should succeed with valid module");
    ASSERT(config.module_count == 1, "module_register: module_count should be 1");
}

void test_module_register_null(void) {
    AgentConfig config;
    util_memset(&config, 0, sizeof(config));

    int rc = module_register(&config, NULL);
    ASSERT(rc == -1, "module_register: NULL mod should return -1");
}

void test_module_find(void) {
    AgentConfig config;
    util_memset(&config, 0, sizeof(config));

    Module *found = module_find(&config, "mock-cmd");
    ASSERT(found != NULL, "module_find: should find 'mock-cmd' after register");
    if (found) {
        ASSERT(util_strcmp(found->name, "mock-cmd") == 0, "module_find: name should match");
    }
}

void test_module_find_missing(void) {
    AgentConfig config;
    util_memset(&config, 0, sizeof(config));

    Module *found = module_find(&config, "nonexistent");
    ASSERT(found == NULL, "module_find: should return NULL for unknown module");
}

void test_module_find_null(void) {
    AgentConfig config;
    util_memset(&config, 0, sizeof(config));

    Module *found = module_find(&config, NULL);
    ASSERT(found == NULL, "module_find: NULL name should return NULL");
}

void test_module_dispatch(void) {
    AgentConfig config;
    util_memset(&config, 0, sizeof(config));
    AgentOutput output;
    util_memset(&output, 0, sizeof(output));

    mock_exec_called = 0;
    mock_exec_result = 0;

    int rc = module_dispatch(&config, "mock-cmd", NULL, 0, &output);
    ASSERT(rc == 0, "module_dispatch: should return 0 for registered module");
    ASSERT(mock_exec_called == 1, "module_dispatch: mock_exec should have been called");
    ASSERT(output.type == OUTPUT_TYPE_TEXT, "module_dispatch: output type should be TEXT");
}

void test_module_dispatch_missing(void) {
    AgentConfig config;
    util_memset(&config, 0, sizeof(config));
    AgentOutput output;
    util_memset(&output, 0, sizeof(output));

    int rc = module_dispatch(&config, "nonexistent", NULL, 0, &output);
    ASSERT(rc == -1, "module_dispatch: should return -1 for unknown module");
    ASSERT(output.type == OUTPUT_TYPE_ERROR, "module_dispatch: output type should be ERROR");
}

void test_module_dispatch_null_output(void) {
    AgentConfig config;
    util_memset(&config, 0, sizeof(config));

    int rc = module_dispatch(&config, "mock-cmd", NULL, 0, NULL);
    ASSERT(rc == -1, "module_dispatch: NULL output should return -1");
}

void register_modules_tests(TESTFUNCS *tests) {
    addTest(tests, test_module_register);
    addTest(tests, test_module_register_null);
    addTest(tests, test_module_find);
    addTest(tests, test_module_find_missing);
    addTest(tests, test_module_find_null);
    addTest(tests, test_module_dispatch);
    addTest(tests, test_module_dispatch_missing);
    addTest(tests, test_module_dispatch_null_output);
}
