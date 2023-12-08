#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/un.h>
#include <sys/socket.h>

#include "client.h"
#include "shared.h"
#include "io_helper.h"

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
EXIT:
        
        return 1;
}