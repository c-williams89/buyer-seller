#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <stdint.h>

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
        // Consider getting the number of entries and looping over that amount
        uint32_t sockfd;
        sockfd = accept(server_sock, (struct sockaddr_un *) &client_sockaddr, &len);

        if (-1 == sockfd) {
                perror("accept on server");
                errno = 0;
                goto EXIT;
        }
        uint8_t byte_array[5] = { 0 };
        // char *end_of_trans = "23"; 
        
        size_t received = recv(sockfd, byte_array, 5, 0);
        while ((received = recv(sockfd, byte_array, 5, 0)) > 0) {
        // while (byte_array[0] != (uint8_t)*EOT) {
                // printf("%d\n", sockfd);
                if (-1 != recv(sockfd, byte_array, 5, 0)) {
                        printf("ACCT: %d\n", byte_array[0]);
                        printf("Transaction: %d\n\n", *(int32_t *)(byte_array + 1));
                } else {
                        printf("No message\n");
                }
        }
        // recv(sockfd, byte_array, 5, 0);
        // printf("ACCT: %d\n", byte_array[0]);
        // printf("Transaction: %d\n", *(int32_t *)(byte_array + 1));
        printf("Connection created with client\n");
EXIT:
        // close(server_sock);   
        return 1;        
}