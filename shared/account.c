#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

typedef struct account_t {
        int amt_owed;
        uint32_t num_orders;
        uint32_t num_payments;
} account_t;

void account_add_order(account_t *acct, int acct_num, int amt) {
        printf("In add order");
}

void account_add_payments(account_t *acct, int acct_num, int amt) {
        printf("In add payment");
}