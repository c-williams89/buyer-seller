#include <check.h>

int main(void) {
        SRunner *sr = srunner_create(NULL);

#ifdef VALGRIND
        srunner_set_fork_status(sr, CK_NOFORK);
#endif
        srunner_run_all(sr, CK_VERBOSE);
        srunner_free(sr);
}