#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/un.h>
#include <sys/socket.h>

#include "shared.h"

int server_create_socket() {
        // struct sockaddr_un server_sockaddr;
        int server_sock = -1;

        server_sock = socket(AF_UNIX, SOCK_STREAM, 0);
        if (-1 == server_sock) {
                perror("server socket creation");
                errno = 0;
                goto EXIT;
        }
EXIT:
        return server_sock;
}