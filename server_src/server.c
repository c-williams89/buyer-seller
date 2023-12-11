#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/un.h>
#include <string.h>
#include <sys/socket.h>

#include "shared.h"

int server_create_socket(void) {
        // struct sockaddr_un server_sockaddr;
        // memset(&server_sockaddr, 0, sizeof(struct sockaddr_un));
        
        // int exit_status = 1;
        int server_sock = -1;

        // status = socket(AF_UNIX, SOCK_STREAM, 0);
        
        server_sock = socket(AF_UNIX, SOCK_STREAM, 0);
        if (-1 == server_sock) {
                perror("server socket creation");
                errno = 0;
                // exit_status = server_sock;
                goto EXIT;
        }

        // server_sockaddr->sun_family = AF_UNIX;
        // strncpy(server_sockaddr->sun_path, SERVER_PATH, sizeof(server_sockaddr->sun_path));

        // socklen_t len = sizeof(server_sockaddr);
        // // exit_status = bind(server_sock, (struct sockaddr *) &server_sockaddr, len);
        // // if (-1 == exit_status) {
        // //         perror("server_bind");
        // //         errno = 0;
        // //         goto EXIT;

        // // }
        // if (-1 == bind(server_sock, (struct sockaddr *) server_sockaddr, len)) {
        //         server_sock = -1;
        //         perror("server connection");
        //         errno = 0;
        //         goto EXIT;
        // }

        // // exit_status = listen(server_sock, 10);
        // // if (-1 == exit_status) {
        // //         perror("server_listen");
        // //         errno = 0;
        // // }
        // if (-1 == listen(server_sock, 10)) {
        //         server_sock = -1;
        //         perror("listen on server");
        //         errno = 0;
        // }
EXIT:
        return server_sock;
}