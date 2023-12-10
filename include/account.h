#ifndef ACCOUNT_H
	#define ACCOUNT_H
	typedef struct account_t {
        	int amt_owed;
        	uint32_t num_orders;
		uint32_t num_payments;
	} account_t;
	account_t * account_create(void);
	void account_add_order(account_t *acct, uint8_t acct_num, int amt);
	void account_add_payments(account_t *acct, int acct_num, int amt);
	void account_print(account_t *acct, uint8_t acct_num);
#endif



