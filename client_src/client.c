#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/un.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdint.h>

#include "shared.h"

int client_create_socket() {

        struct sockaddr_un client_sockaddr;
        int client_sock = -1;

        client_sock = socket(AF_UNIX, SOCK_STREAM, 0);
        if (-1 == client_sock){
                perror("client_create_socket");
                errno = 0;
                goto EXIT;
        }

        client_sockaddr.sun_family = AF_UNIX;
        strncpy(client_sockaddr.sun_path, CLIENT_PATH, sizeof(client_sockaddr.sun_path));
        unlink(CLIENT_PATH);

EXIT:
        return client_sock;
}