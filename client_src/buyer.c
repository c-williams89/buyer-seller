#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/un.h>
#include <sys/socket.h>
#include <stdint.h>

#include "client.h"
#include "shared.h"
#include "io_helper.h"

int main (int argc, char *argv[]) {
        // char *test = "4";
        // long val = strtol(test, NULL, 10);
        // printf("%b\n", val);


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
        
        while (-1 != getline(&buff, &len, fp)) {
                char *cpy = buff;
                uint8_t byte_array[5] = { 0 };
                char *arg = strtok(cpy, " ");
                byte_array[0] = (uint8_t)*arg - '0';
                // printf("index 0: %d\n", byte_array[0]);
                // printf("%s\n", cpy);
                arg = strtok(NULL, "\n");
                long new_val = strtol(arg, NULL, 10);
                int32_t *num = (byte_array + 1);
                *num = (int32_t)new_val;
                printf("%d\n", *num);
                printf("%s\n", arg);
                for (int i = 0; i < 5; ++i) {
                        printf("%02x ", byte_array[i]);
                }
                // send()
                puts("");
                break;
        }
        free(buff);
EXIT:
        
        return 1;
}