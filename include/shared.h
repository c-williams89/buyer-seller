// defines for shared behavior between client and server
#ifndef SHARED_H
#define SHARED_H
#include <stdint.h>

/**
 * Struct to account information. 
 */
typedef struct account_t {
	int amt_owed;
	uint32_t num_orders;
	uint32_t num_payments;
} account_t;

// Maximum number of accounts
#define NUM_ACCTS 5

// Maximum number of clients the server accepts communication from
#define NUM_MAX_CLIENTS 10

// Pathname for server Unix Domain Socket
#define SERVER_PATH "server_unix_domain_socket"

// Pathname for client Unix Domain Socket
#define CLIENT_PATH "client_unix_domain_socket"
#endif
