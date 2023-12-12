#ifndef CLIENT_HELPER_H
#define CLIENT_HELPER_H
/**
 * @brief Validates a file path and validates against regular files of size > 0
 * 
 * @param fp File pointer to validate
 * 
 * @return 1 if valide, else 0
 */
int validate_file(FILE * fp);

/**
 * @brief Creates a Unix Domain Socket
 * 
 * @return sockfd fd to new socket, or -1 on failure.
 */
int client_create_socket();
#endif
