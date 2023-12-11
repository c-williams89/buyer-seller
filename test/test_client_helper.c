#include <check.h>
#include <stdio.h>
#include <stdlib.h>

#include "client_helper.h"

START_TEST (test_validate_file_invalid) {
        char *invalid_files[] = {
                "/dev/null",
                "/dev/urandom",
                "/dev/zero",
                "./test/",
                "./test/test_data/0_file"
        };

        for (int i = 0; i < 5; ++i) {
                FILE *fp = fopen(invalid_files[i], "r");
                ck_assert_int_eq(validate_file(fp), 0);
        }
} END_TEST

START_TEST (test_validate_file_valid) {
        char *valid_file = "./data/seller1.dat";
        FILE *fp = fopen(valid_file, "r");
        ck_assert_int_eq(validate_file(fp), 1);
}END_TEST

START_TEST (test_client_create_socket) {
        ck_assert_int_ne(client_create_socket(), -1);
        ck_assert_int_ge(client_create_socket(), 2);
}END_TEST

TFun client_helper_tests[] = {
        test_validate_file_invalid,
        test_validate_file_valid,
        test_client_create_socket,
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