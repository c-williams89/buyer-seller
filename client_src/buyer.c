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

        account_t *client_accounts[NUM_ACCTS];
        for (int i = 0; i < NUM_ACCTS; ++i) {
                account_t *account;
                memset(&account, 0, sizeof(account));
                client_accounts[i] = &account;
        }
        
        printf("Client is Running\n");
        
        struct sockaddr_un server_sockaddr;
        // struct sockaddr_un client_sockaddr;
	char *server_socket_path = "server_unix_domain_socket";

        int client_sock = client_create_socket();
        if (-1 == client_sock) {
                goto EXIT;
        }

        server_sockaddr.sun_family = AF_UNIX;
        int len = sizeof(server_sockaddr);
        strncpy(server_sockaddr.sun_path, server_socket_path, strlen(server_socket_path));
        printf("%s\n", server_sockaddr.sun_path);

        if (-1 == connect(client_sock, (struct sockaddr *) &server_sockaddr, len)) {
                perror("client connection");
                errno = 0;
                goto EXIT;
        }

        char *buff = NULL;
        size_t buff_len = 0;
        uint8_t byte_array[5] = { 0 };
        while (-1 != getline(&buff, &buff_len, fp)) {
                printf("Beginning of while\n");
                char *cpy = buff;
                char *arg = strtok(cpy, " ");
                // int acct_num = strtol(arg, NULL, 10);
                uint8_t acct_num = (uint8_t)*arg - '0';
                // byte_array[0] = (uint8_t)*arg - '0';
                byte_array[0] = acct_num;
                arg = strtok(NULL, "\n");
                long new_val = strtol(arg, NULL, 10);
                int32_t *num = (byte_array + 1);
                *num = (int32_t)new_val;
                /*
                Before send, vals are:
                arg = 36, first iter 9890
                cpy = 4
                new_val = 36 addr deb0
                buff = 4, same address as cpy
                */
                // printf("%d\n", acct_num);
                // printf("%d\n", new_val);
                // if (client_accounts[acct_num]) {
                printf("Before add\n");
                account_add_order(client_accounts[acct_num], acct_num, new_val);
                printf("Added to account\n");

                // }
                printf("Before send\n");
                send(client_sock, byte_array, 5, 0);
                printf("After send\n");
        }
        for (int i = 0; i < NUM_ACCTS; ++i) {
                if (client_accounts[i]) {
                        account_print(client_accounts[i], i);
                        // printf("%d\n", i);
                }
        }
        free(buff);

        // printf("before close: %d\n", client_sock);

        close(client_sock);
        // printf("after close: %d\n", client_sock);
        fclose(fp);
EXIT:
        return 1;
}