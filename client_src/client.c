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
#include <time.h>
#include <stdbool.h>

#include "client_helper.h"
#include "shared.h"

sig_atomic_t SIGINT_FLAG = 0;
jmp_buf env;
bool clock_time = false;

static void handle_SIGINT(int signum)
{
	(void)signum;
	longjmp(env, 1);
	SIGINT_FLAG = 1;
}

static void regist_signal_handler(void)
{
	struct sigaction sigint_action;
	sigset_t sigint_set;
	sigemptyset(&sigint_set);
	sigint_action.sa_flags = 0;
	sigint_action.sa_handler = handle_SIGINT;
	sigint_action.sa_mask = sigint_set;
	sigaction(SIGINT, &sigint_action, NULL);
}

int main(int argc, char *argv[])
{
	regist_signal_handler();
	struct sockaddr_un server_sockaddr;

	if (1 == argc) {
		fprintf(stderr, "client: Missing file argument\n");
		goto EXIT;
	}
	if (argc > 3) {
		fprintf(stderr, "client: Too many arguments\n");
		goto EXIT;
	}
	if (3 == argc) {
		if ((0 == strncmp(argv[2], "-p", 3))) {
			clock_time = true;
		} else {
			fprintf(stderr, "client: invalid option -- '%s'\n",
				argv[2]);
			goto EXIT;
		}
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
	uint32_t packet_sent = 0;
	clock_t t = 0;
	int total_time = 0;
	double trans_per_sec = 0.0;

	while (-1 != getline(&buff, &buff_len, fp)) {
		if (SIGINT_FLAG) {
			fprintf(stderr, "SIGNAL INTERRUPT: shutting down.\n");
			break;
		}
		if (0 == (packet_sent % 10000)) {
			sleep(1);
		}

		char *cpy = buff;
		char *arg = strtok(cpy, " ");
		uint8_t acct_num = (uint8_t) * arg - '0';
		byte_array[0] = acct_num;
		arg = strtok(NULL, "\n");
		long new_val = strtol(arg, NULL, 10);
		int32_t *num = (int32_t *) (byte_array + 1);
		*num = (int32_t) new_val;

		if (0 == new_val) {
			continue;
		}

		t = clock();
		if (new_val > 0) {
			client_accounts[acct_num - 1].num_orders += 1;
		} else {
			client_accounts[acct_num - 1].num_payments += 1;
		}
		client_accounts[acct_num - 1].amt_owed += new_val;

		if (-1 == send(client_sock, byte_array, 5, 0)) {
			perror("client");
			errno = 0;
		}

		t = clock() - t;
		total_time += t;
		packet_sent++;
	}

	trans_per_sec =
	    (double)packet_sent / ((double)total_time / CLOCKS_PER_SEC);
	for (int i = 0; i < NUM_ACCTS; ++i) {
		if (client_accounts[i].num_orders > 0) {
			printf("%-20s %u %7d %5u %5u\n", argv[1],
			       i + 1,
			       client_accounts[i].amt_owed,
			       client_accounts[i].num_orders,
			       client_accounts[i].num_payments);
		}
	}
	if (clock_time) {
		printf("Transmissions per second: %.2f\n", trans_per_sec);
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
