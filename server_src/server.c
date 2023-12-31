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
#include <time.h>
#include <stdbool.h>

#include "server_helper.h"
#include "shared.h"

sig_atomic_t SIGINT_FLAG = 0;
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
bool b_is_process_flag = false;

typedef struct pkg_t {
	account_t *client_accounts;
	ssize_t sockfd;
} pkg_t;

static void handle_SIGINT(int signum)
{
	(void)signum;
	SIGINT_FLAG = 1;
}

static void register_signal_handler(void)
{
	struct sigaction sigint_action;
	sigset_t sigint_set;
	sigemptyset(&sigint_set);
	sigint_action.sa_flags = 0;
	sigint_action.sa_handler = handle_SIGINT;
	sigint_action.sa_mask = sigint_set;
	sigaction(SIGINT, &sigint_action, NULL);
}

void *thread_func(void *arg)
{
	if (!arg) {
		fprintf(stderr, "server: NULL thread arg\n");
		return NULL;
	}
	uint8_t byte_array[5] = { 0 };
	pkg_t *pkg = (pkg_t *) arg;
	account_t *client_accounts = pkg->client_accounts;
	size_t received = 0;
	uint32_t packets = 0;
	clock_t t = 0;
	int total_time = 0;
	double trans_per_sec = 0.0;
	t = clock();
	while ((received = recv(pkg->sockfd, byte_array, 5, 0)) > 0) {
		if (-1 == (int)received) {
			perror("server_receive");
			errno = 0;
		}

		/**
		 * Deserialize the byte array into integer values that can be
		 * assigned to member variables of the account_t struct. 
		 */
		uint8_t acct_num = byte_array[0];
		int32_t *amt_owed = (int32_t *) (byte_array + 1);

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
		packets++;
	}

	total_time = clock() - t;
	trans_per_sec = (double)packets / ((double)total_time / CLOCKS_PER_SEC);
	if (b_is_process_flag) {
		printf("Socket Connection %ld transaction data:\t%.2f t/s\n",
		       pkg->sockfd, trans_per_sec);
	}
	close(pkg->sockfd);
	free(pkg);
	return NULL;
}

int main(int argc, char *argv[])
{
	register_signal_handler();
	int exit_status = 1;

	if (argc > 2) {
		fprintf(stderr, "server: Too many arguments\n");
		goto ARG_EXIT;
	} else {
		if (2 == argc) {
			if ((0 == strncmp(argv[1], "-p", 3))) {
				b_is_process_flag = true;
			} else {
				fprintf(stderr,
					"server: invalid option -'%s'\n",
					argv[1]);
				goto ARG_EXIT;
			}
		}
	}

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

	// Create a UNIX domain stream socket
	int server_sock = server_create_socket();
	if (-1 == server_sock) {
		fprintf(stderr, "Error creating server socket\n");
		goto EXIT;
	}

	/**
	 * Set up the UNIX sockaddr structure with AF_UNIX for the family and 
	 * filepath for server path defined in shared.h to bind to. 
	 */
	server_sockaddr.sun_family = AF_UNIX;
	strncpy(server_sockaddr.sun_path, SERVER_PATH,
		sizeof(server_sockaddr.sun_path));

	if (-1 == bind(server_sock, (struct sockaddr *)&server_sockaddr, len)) {
		perror("server connection");
		errno = 0;
		goto EXIT;
	}
	// Listen for any client sockets.
	if (-1 == listen(server_sock, 10)) {
		perror("listen on server");
		errno = 0;
		goto EXIT;
	}

	printf("Server: Waiting for connections...\n");

	/**
	 * Server will accept a total of 10 client connections, either
	 * individually or at once. With each successful accept, create a
	 * thread for the new connection. 
	 */
	exit_status = 0;
	while (num_connected < NUM_MAX_CLIENTS) {
		if (SIGINT_FLAG) {
			fprintf(stderr, "SIGNAL INTERRUPT: shutting down.\n");
			exit_status = 1;
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
		if (!pkg) {
			perror("server");
			errno = 0;
			exit_status = 1;
			break;
		}
		pkg->client_accounts = client_accounts;
		pkg->sockfd = sockfd;

		pthread_create(thread_list + num_connected, NULL, thread_func,
			       (void *)pkg);
		num_connected++;
	}

	/**
	 * Join all threads after the server has accepted 10 client threads. If
	 * server was terminated prior to all 10 threads executing, print error
	 * message that thread was not created. 
	 */
	for (int i = 0; i < NUM_MAX_CLIENTS; ++i) {
		if (pthread_join(thread_list[i], NULL)) {
			fprintf(stderr, "Error joining thread\n");
		}
	}

	/**
	 * Print account information for each account using the format:
	 * %-20s %u %7d %5u %5u\n
	 */
	for (int i = 0; i < NUM_ACCTS; ++i) {
		printf("%-20s %u %7d %5u %5u\n", "network",
		       i + 1,
		       client_accounts[i].amt_owed,
		       client_accounts[i].num_orders,
		       client_accounts[i].num_payments);
	}

 EXIT:
	close(server_sock);
 ARG_EXIT:
	return exit_status;
}
