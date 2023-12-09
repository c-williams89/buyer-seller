#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <errno.h>

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
