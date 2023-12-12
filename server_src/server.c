#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <stdint.h>
#include <pthread.h>
#include <unistd.h>
#include <signal.h>

#include "server_helper.h"
#include "shared.h"

sig_atomic_t SIGINT_FLAG = 0;
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

typedef struct pkg_t {
	account_t *client_accounts;	
	ssize_t sockfd;
} pkg_t;

static void handle_SIGINT(int signum)
{
	(void)signum;
	SIGINT_FLAG = 1;
}

void *thread_func(void *arg)
{
	uint8_t byte_array[5] = { 0 };
	pkg_t *pkg = (pkg_t *) arg;
	account_t *client_accounts = pkg->client_accounts;
	size_t received = 0;
	while ((received = recv(pkg->sockfd, byte_array, 5, 0)) > 0) {
		if (-1 == (int)received) {
			perror("server_receive");
			errno = 0;
		}
		uint8_t acct_num = byte_array[0];
		int32_t *amt_owed = (int32_t *)(byte_array + 1);

		if (0 == amt_owed) {
			continue;
		}
		pthread_mutex_lock(&lock);
		if (*amt_owed > 0) {
			client_accounts[acct_num - 1].num_orders += 1;
		} else {
			client_accounts[acct_num - 1].num_payments += 1;
		}
		client_accounts[acct_num - 1].amt_owed += *amt_owed;
		pthread_mutex_unlock(&lock);
	}
	free(pkg);
	return NULL;
}

int main(void)
{
	// Register signal handler for SIGINT
	struct sigaction sigint_action;
	sigset_t sigint_set;
	sigemptyset(&sigint_set);
	sigint_action.sa_flags = 0;
	sigint_action.sa_handler = handle_SIGINT;
	sigint_action.sa_mask = sigint_set;
	sigaction(SIGINT, &sigint_action, NULL);

	account_t client_accounts[NUM_ACCTS] = { 0 };
	for (int i = 0; i < NUM_ACCTS; ++i) {
		account_t account = { 0, 0, 0 };
		client_accounts[i] = account;
	}

	pthread_t thread_list[NUM_MAX_CLIENTS] = { 0 };
	uint32_t num_connected = 0;

	struct sockaddr_un server_sockaddr;
	memset(&server_sockaddr, 0, sizeof(struct sockaddr_un));

	struct sockaddr_un client_sockaddr;
	memset(&client_sockaddr, 0, sizeof(struct sockaddr_un));
	socklen_t len = sizeof(server_sockaddr);

	int server_sock = server_create_socket();
	if (-1 == server_sock) {
		fprintf(stderr, "Error creating server socket\n");
		goto EXIT;
	}
	server_sockaddr.sun_family = AF_UNIX;
	strncpy(server_sockaddr.sun_path, SERVER_PATH,
		sizeof(server_sockaddr.sun_path));

	if (-1 == bind(server_sock, (struct sockaddr *)&server_sockaddr, len)) {
		perror("server connection");
		errno = 0;
		goto EXIT;
	}

	if (-1 == listen(server_sock, 10)) {
		perror("listen on server");
		errno = 0;
		goto EXIT;
	}

	printf("Server: Waiting for connections...\n");

	while (num_connected < NUM_MAX_CLIENTS) {
		if (SIGINT_FLAG) {
			fprintf(stderr, "SIGNAL INTERRUPT: shutting down.\n");
			break;
		}
		uint32_t sockfd =
		    accept(server_sock, (struct sockaddr *)&client_sockaddr,
			   &len);
		if (-1 == (int)sockfd) {
			// Handle errno 4, EINTR: Interrupted System Call
			if (4 == errno) {
				continue;
			}
			perror("accept on server");
			errno = 0;
			continue;
		}
		pkg_t *pkg = calloc(1, sizeof(*pkg));
		pkg->client_accounts = client_accounts;
		pkg->sockfd = sockfd;

		pthread_create(thread_list + num_connected, NULL, thread_func,
			       (void *)pkg);
		num_connected++;
	}

	for (int i = 0; i < NUM_MAX_CLIENTS; ++i) {
		if (pthread_join(thread_list[i], NULL)) {
			fprintf(stderr, "Error joining thread\n");
		}
	}

	for (int i = 0; i < NUM_ACCTS; ++i) {
		printf("%-20s %u %7d %5u %5u\n", "network", 
		       i + 1,
		       client_accounts[i].amt_owed,
		       client_accounts[i].num_orders,
		       client_accounts[i].num_payments);
	}
 EXIT:
	close(server_sock);
	return 1;
}
