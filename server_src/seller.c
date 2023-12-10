#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <stdint.h>
#include <pthread.h>

#include "server.h"
#include "shared.h"
#include "account.h"

#define NUM_MAX_CLIENTS 10
#define NUM_ACCTS 5

typedef struct pkg_t {
        account_t *client_accounts;
        ssize_t sockfd;
} pkg_t;

void *thread_func(void *arg) {
        printf("thread created\n");
        uint8_t byte_array[5] = { 0 };
        pkg_t *pkg = (pkg_t *)arg;
        account_t *client_accounts = pkg->client_accounts;

        size_t received = 0;
        while ((received = recv(pkg->sockfd, byte_array, 5, 0)) > 0) {
                if (-1 == received) {
                        perror("server_receive");
                        errno = 0;
                }
                uint8_t acct_num = byte_array[0];
                int32_t amt_owed = (byte_array + 1);
                // client_accounts[acct_num - 1].amt_owed += amt_owed;
                client_accounts[acct_num - 1].amt_owed += *(int32_t *)(byte_array + 1);

                // client_accounts[acct_num]->amt_owed += byte_array + 1;
                // printf("Transaction: %d\n\n", *(int32_t *)(byte_array + 1));
        }
}

int main(void) {
        printf("Server is running\n");

        account_t client_accounts[NUM_ACCTS] = { 0 };
        for (int i = 0; i < NUM_ACCTS; ++i) {
                account_t account = {0, 0, 0};
                client_accounts[i] = account;
        }

        pthread_t thread_list[NUM_MAX_CLIENTS] = { 0 };
        uint32_t num_connected = 0;

        struct sockaddr_un server_sockaddr;
        struct sockaddr_un client_sockaddr;
        memset(&client_sockaddr, 0, sizeof(struct sockaddr_un));
        int server_sock = server_create_socket();
        if (-1 == server_sock) {
                goto EXIT;
        }
        server_sockaddr.sun_family = AF_UNIX;
        strncpy(server_sockaddr.sun_path, SERVER_PATH, strlen(SERVER_PATH));

        int len = sizeof(server_sockaddr);
        if (-1 == bind(server_sock, (struct sockaddr_un *) &server_sockaddr, len)) {
                perror("server connection");
                errno = 0;
                goto EXIT;
        }

        if (-1 == listen(server_sock, 100)) {
                perror("listen on server");
                errno = 0;
                goto EXIT;
        }

        uint32_t sockfd;
        sockfd = accept(server_sock, (struct sockaddr_un *) &client_sockaddr, &len);

        if (-1 == sockfd) {
                perror("accept on server");
                errno = 0;
                goto EXIT;
        }
        pkg_t pkg;
        pkg.client_accounts = &client_accounts;
        pkg.sockfd = sockfd;

        pthread_create(thread_list + num_connected, NULL, thread_func, (void *)&pkg);

        // uint8_t byte_array[5] = { 0 };
        
        // size_t received = 0;
        // while ((received = recv(sockfd, byte_array, 5, 0)) > 0) {
        //         if (-1 == received) {
        //                 perror("server_receive");
        //                 errno = 0;
        //         }
        // }
        pthread_join(thread_list[0], NULL);
        printf("Connection created with client\n");
        for (int i = 0; i < NUM_ACCTS; ++i) {
                printf("%d\t%d  %d  %d\n", i, client_accounts[i].amt_owed, client_accounts[i].num_orders, client_accounts[i].num_payments);
        }
EXIT:
        // close(server_sock);   
        return 1;        
}