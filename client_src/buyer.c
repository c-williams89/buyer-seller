#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/un.h>
#include <sys/socket.h>
#include <stdint.h>
#include <unistd.h>

#include "client.h"
#include "shared.h"
#include "io_helper.h"
#include "account.h"

#define NUM_ACCTS 5

int main (int argc, char *argv[]) {
        struct sockaddr_un server_sockaddr;
        printf("file is: %s\n", argv[1]);

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
                goto EXIT;
        }

        account_t client_accounts[NUM_ACCTS] = { 0 };
        for (int i = 0; i < NUM_ACCTS; ++i) {
                account_t account = {0, 0, 0};
                client_accounts[i] = account;
        }
        
        printf("Client is Running\n");
        
	// char *server_socket_path = "server_unix_domain_socket";

        int client_sock = client_create_socket();
        if (-1 == client_sock) {
                goto EXIT;
        }

        server_sockaddr.sun_family = AF_UNIX;
        int len = sizeof(server_sockaddr);
        strncpy(server_sockaddr.sun_path, SERVER_PATH, strlen(SERVER_PATH));

        if (-1 == connect(client_sock, (struct sockaddr *) &server_sockaddr, len)) {
                perror("client connection");
                errno = 0;
                goto EXIT;
        }

        char *buff = NULL;
        size_t buff_len = 0;
        uint8_t byte_array[5] = { 0 };
        while (-1 != getline(&buff, &buff_len, fp)) {
                char *cpy = buff;
                char *arg = strtok(cpy, " ");
                uint8_t acct_num = (uint8_t)*arg - '0';
                // byte_array[0] = (uint8_t)*arg - '0';
                byte_array[0] = acct_num;
                arg = strtok(NULL, "\n");
                long new_val = strtol(arg, NULL, 10);
                int32_t *num = (byte_array + 1);
                *num = (int32_t)new_val;
                if (0 == new_val) {
                        continue;
                } else if (new_val > 0) {
                        client_accounts[acct_num - 1].num_orders += 1;
                } else {
                        client_accounts[acct_num -1].num_payments += 1;
                }
                client_accounts[acct_num - 1].amt_owed += new_val;

                send(client_sock, byte_array, 5, 0);
        }
        for (int i = 0; i < NUM_ACCTS; ++i) {
                printf("%s\t%d  %d  %d  %d\n", argv[1], i + 1, client_accounts[i].amt_owed, client_accounts[i].num_orders, client_accounts[i].num_payments);
        }
        
        free(buff);
        close(client_sock);
        fclose(fp);
EXIT:
        return 1;
}