#ifndef SERVER_HELPER_H
#define SERVER_HELPER_H
/**
 * @brief Creates a Unix Domain Socket
 * 
 * @return sockfd fd to new socket, or -1 on failure.
 */
int server_create_socket(void);
#endif
