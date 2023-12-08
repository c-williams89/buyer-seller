#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/socket.h>
#include <sys/un.h>

#include "server.h"
#include "shared.h"

int main(void) {
        printf("Server is running\n");
        
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

        if (-1 == accept(server_sock, (struct sockaddr_un *) &client_sockaddr, &len)) {
                perror("accept on server");
                errno = 0;
                goto EXIT;
        } else {
                printf("Connection created with client\n");
        }
EXIT:
        // close(server_sock);   
        return 1;        
}