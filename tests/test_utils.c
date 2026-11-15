#include <windows.h>
#include "../agent/libs/LibCPLTest/src/cpltest.h"
#include "../agent/libs/LibUtils/src/utils.h"

void test_util_strlen_empty(void) {
    int len = util_strlen("");
    ASSERT(len == 0, "util_strlen: empty string should return 0");
}

void test_util_strlen_null(void) {
    int len = util_strlen(NULL);
    ASSERT(len == 0, "util_strlen: NULL should return 0");
}

void test_util_strlen_normal(void) {
    int len = util_strlen("hello");
    ASSERT(len == 5, "util_strlen: 'hello' should return 5");
}

void test_util_strcmp_equal(void) {
    int rc = util_strcmp("abc", "abc");
    ASSERT(rc == 0, "util_strcmp: equal strings should return 0");
}

void test_util_strcmp_less(void) {
    int rc = util_strcmp("abc", "abd");
    ASSERT(rc < 0, "util_strcmp: 'abc' < 'abd'");
}

void test_util_strcmp_greater(void) {
    int rc = util_strcmp("abd", "abc");
    ASSERT(rc > 0, "util_strcmp: 'abd' > 'abc'");
}

void test_util_strcmp_empty(void) {
    int rc = util_strcmp("", "");
    ASSERT(rc == 0, "util_strcmp: two empty strings should be equal");
}

void test_util_itoa_positive(void) {
    char buf[32];
    util_itoa(buf, 12345);
    ASSERT(util_strcmp(buf, "12345") == 0, "util_itoa: 12345");
}

void test_util_itoa_zero(void) {
    char buf[32];
    util_itoa(buf, 0);
    ASSERT(util_strcmp(buf, "0") == 0, "util_itoa: 0");
}

void test_util_itoa_negative(void) {
    char buf[32];
    util_itoa(buf, -42);
    ASSERT(util_strcmp(buf, "-42") == 0, "util_itoa: -42");
}

void test_util_hex(void) {
    char buf[32];
    util_hex(buf, 0xDEADBEEF);
    ASSERT(buf[0] == '0', "util_hex: should start with '0x'");
    ASSERT(buf[1] == 'x', "util_hex: should start with '0x'");
    int len = util_strlen(buf);
    ASSERT(len == 18, "util_hex: output length should be 18");
}

void test_util_atoi_positive(void) {
    int val = util_atoi("9000");
    ASSERT(val == 9000, "util_atoi: '9000' should return 9000");
}

void test_util_atoi_negative(void) {
    int val = util_atoi("-100");
    ASSERT(val == -100, "util_atoi: '-100' should return -100");
}

void test_util_atoi_null(void) {
    int val = util_atoi(NULL);
    ASSERT(val == 0, "util_atoi: NULL should return 0");
}

void test_util_atoi_zero(void) {
    int val = util_atoi("0");
    ASSERT(val == 0, "util_atoi: '0' should return 0");
}

void test_util_memset(void) {
    char buf[16];
    util_memset(buf, 0x41, 8);
    ASSERT(buf[0] == 0x41, "util_memset: first byte should be 0x41");
    ASSERT(buf[7] == 0x41, "util_memset: last byte should be 0x41");
}

void test_util_memcpy(void) {
    char src[] = "ABCDEFGH";
    char dst[16];
    util_memset(dst, 0, 16);
    util_memcpy(dst, src, 8);
    ASSERT(dst[0] == 'A', "util_memcpy: first byte");
    ASSERT(dst[7] == 'H', "util_memcpy: last byte");
    ASSERT(dst[8] == 0, "util_memcpy: should not overrun");
}

void register_utils_tests(TESTFUNCS *tests) {
    addTest(tests, test_util_strlen_empty);
    addTest(tests, test_util_strlen_null);
    addTest(tests, test_util_strlen_normal);
    addTest(tests, test_util_strcmp_equal);
    addTest(tests, test_util_strcmp_less);
    addTest(tests, test_util_strcmp_greater);
    addTest(tests, test_util_strcmp_empty);
    addTest(tests, test_util_itoa_positive);
    addTest(tests, test_util_itoa_zero);
    addTest(tests, test_util_itoa_negative);
    addTest(tests, test_util_hex);
    addTest(tests, test_util_atoi_positive);
    addTest(tests, test_util_atoi_negative);
    addTest(tests, test_util_atoi_null);
    addTest(tests, test_util_atoi_zero);
    addTest(tests, test_util_memset);
    addTest(tests, test_util_memcpy);
}
