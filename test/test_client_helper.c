#include <check.h>

TFun client_helper_tests[] = {
        NULL
};

Suite *test_client_helper(void) {
        Suite *s = suite_create("test_client_helper");
        TFun *curr = NULL;
        TCase *tc_core = tcase_create("client_helper");

        curr = client_helper_tests;
        while (*curr) {
                tcase_add_test(tc_core, *curr++);
        }
        suite_add_tcase(s, tc_core);
        return s;
}