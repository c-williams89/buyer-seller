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
#include <sys/stat.h>

#include "shared.h"

int validate_file(FILE * fp)
{
	int exit_status = 0;

	struct stat path_stat;
	int fd = fileno(fp);
	if (-1 == fstat(fd, &path_stat)) {
		perror("client");
		errno = 0;
		goto EXIT;
	}

	if (S_ISREG(path_stat.st_mode)) {
		if (0 < path_stat.st_size) {
			exit_status = 1;
		}
	}

 EXIT:
	return exit_status;
}

int client_create_socket()
{
	/**
	 * Create the UNIX domain stream socket. Set up UNIX sockaddr structure
	 * using AF_UNIX for family and defined filepath to bind to (shared.h).
	 * 
	 * Unlink file so the bind will succeed on the server side. 
	 */
	struct sockaddr_un client_sockaddr;
	int client_sock = -1;

	client_sock = socket(AF_UNIX, SOCK_STREAM, 0);
	if (-1 == client_sock) {
		perror("client_create_socket");
		errno = 0;
		goto EXIT;
	}

	client_sockaddr.sun_family = AF_UNIX;
	strncpy(client_sockaddr.sun_path, CLIENT_PATH,
		sizeof(client_sockaddr.sun_path));
	unlink(CLIENT_PATH);

 EXIT:
	return client_sock;
}
