#include <check.h>
/*
gcc test/*.c client_src/client_helper.c server_src/server_helper.c -I./include -lcheck -lm -pthread -lrt -lsubunit

*/

extern Suite *test_client_helper(void);

int main(void) {
        SRunner *sr = srunner_create(NULL);

        Suite *test_client_helper_suite = test_client_helper();

        srunner_add_suite(sr, test_client_helper_suite);

#ifdef VALGRIND
        srunner_set_fork_status(sr, CK_NOFORK);
#endif
        srunner_run_all(sr, CK_VERBOSE);
        srunner_free(sr);
}