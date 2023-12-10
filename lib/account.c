#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

/*
To create the .a for statically linked library.
gcc -c account.c
ar -rc shared.a account.o
*/

typedef struct account_t {
        int amt_owed;
        uint32_t num_orders;
        uint32_t num_payments;
} account_t;

void account_add_order(account_t *acct, uint8_t acct_num, int amt) {
        // if (!acct || !*acct) {
        //         printf("NULL");
        //         return;
        // }
        if (!acct) {
                printf("NULL");
        }
        printf("adding to account\n");
        acct->amt_owed += amt;
}

void account_add_payments(account_t *acct, int acct_num, int amt) {
        acct->amt_owed += amt;
        printf("In add payment");
}

void account_print(account_t *acct, uint8_t acct_num) {
        printf("%d\t%d\n", acct_num, acct->amt_owed);
}