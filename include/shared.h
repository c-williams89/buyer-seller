#ifndef SHARED_H
#define SHARED_H
#include <stdint.h>
typedef struct account_t {
	int amt_owed;
	uint32_t num_orders;
	uint32_t num_payments;
} account_t;
#define EOT "23"
#define NUM_ACCTS 5
#define NUM_MAX_CLIENTS 10
#define SERVER_PATH "server_unix_domain_socket"
#define CLIENT_PATH "client_unix_domain_socket"
#endif
