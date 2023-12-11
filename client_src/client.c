#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/un.h>
#include <sys/socket.h>
#include <stdint.h>
#include <unistd.h>
#include <signal.h>
#include <setjmp.h>

#include "client_helper.h"
#include "shared.h"

sig_atomic_t SIGINT_FLAG = 0;
jmp_buf env;

static void handle_SIGINT(int signum)
{
	(void)signum;
	longjmp(env, 1);
	SIGINT_FLAG = 1;
}

int main(int argc, char *argv[])
{
	// Set up signal handler
	struct sigaction sigint_action;
	sigset_t sigint_set;
	sigemptyset(&sigint_set);
	// sigaddset(&sigint_set, )
	sigint_action.sa_flags = 0;
	sigint_action.sa_handler = handle_SIGINT;
	sigint_action.sa_mask = sigint_set;
	sigaction(SIGINT, &sigint_action, NULL);

	struct sockaddr_un server_sockaddr;

	if (1 == argc) {
		fprintf(stderr, "client: Missing file argument\n");
		goto EXIT;
	}

	if (argc > 2) {
		fprintf(stderr, "client: Too many arguments\n");
		goto EXIT;
	}

	FILE *fp = fopen(argv[1], "r");
	if (!fp) {
		perror("client");
		errno = 0;
		goto EXIT;
	}

	if (!validate_file(fp)) {
		goto FILE_EXIT;
	}

	account_t client_accounts[NUM_ACCTS] = { 0 };
	for (int i = 0; i < NUM_ACCTS; ++i) {
		account_t account = { 0, 0, 0 };
		client_accounts[i] = account;
	}

	int client_sock = client_create_socket();
	if (-1 == client_sock) {
		goto EXIT;
	}

	server_sockaddr.sun_family = AF_UNIX;
	int len = sizeof(server_sockaddr);
	strncpy(server_sockaddr.sun_path, SERVER_PATH,
		sizeof(server_sockaddr.sun_path));

	if (-1 ==
	    connect(client_sock, (struct sockaddr *)&server_sockaddr, len)) {
		perror("client connection");
		errno = 0;
		goto FILE_EXIT;
	}

	char *buff = NULL;
	size_t buff_len = 0;
	uint8_t byte_array[5] = { 0 };
	while (-1 != getline(&buff, &buff_len, fp)) {
		if (SIGINT_FLAG) {
			fprintf(stderr, "SIGNAL INTERRUPT: shutting down.\n");
			break;
		}
		// TODO: Consider adding all this in a helper function
		//  serialize_data, and deserialize_data on server side
		//  to mock a custom protocol
		char *cpy = buff;
		char *arg = strtok(cpy, " ");
		uint8_t acct_num = (uint8_t) * arg - '0';
		byte_array[0] = acct_num;
		arg = strtok(NULL, "\n");
		long new_val = strtol(arg, NULL, 10);
		int32_t *num = (byte_array + 1);
		*num = (int32_t) new_val;

		if (0 == new_val) {
			continue;
		} else if (new_val > 0) {
			client_accounts[acct_num - 1].num_orders += 1;
		} else {
			client_accounts[acct_num - 1].num_payments += 1;
		}
		client_accounts[acct_num - 1].amt_owed += new_val;

		if (-1 == send(client_sock, byte_array, 5, 0)) {
			perror("client");
			errno = 0;
		}
	}
	for (int i = 0; i < NUM_ACCTS; ++i) {
		if (client_accounts[i].num_orders > 0) {
			printf("%s\t%d  %d  %d  %d\n", argv[1], i + 1,
			       client_accounts[i].amt_owed,
			       client_accounts[i].num_orders,
			       client_accounts[i].num_payments);
		}
	}
	puts("");
	free(buff);
	close(client_sock);
	unlink(CLIENT_PATH);
 FILE_EXIT:
	fclose(fp);
 EXIT:
	return 1;
}
