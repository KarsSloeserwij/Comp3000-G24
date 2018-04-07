//Modified 2.0
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>
#include <myencrypt.h>
#include <fcntl.h>
#include <string.h>

int main(int argc, char **argv)
{
    /* parse arguments (mode (-e,-d), fileName, password) */
	if (argc < 4) {
		// error
		fprintf(stderr, "Usage: encr mode(-e,-d) path/fileName password\n");
		exit(-1);
	}

    /* check mode */
    if(!(strcmp(argv[1],"-e") == 0 || (strcmp(argv[1],"-d") == 0))) {
        // error
		fprintf(stderr, "Wrong option for mode. -e to encrypt, -d to decrypt\n");
		exit(-1);
    }

    /* check if file exists */
    int fd = open (argv[2], O_RDWR);
    /* check if opened successfully */
    if (fd < 0) {
        fprintf(stderr, "open error: %s\n", strerror(errno));
        exit(-1);
    }
	close (fd);

	/* no errors from input */

	// get full filePath
	char filepath[256];
	int rc;
	realpath(argv[2], filepath);

    if (strcmp(argv[1],"-e") == 0) {
        rc = myencrypt(1, filepath, argv[3]);   // 1 for encryption
		if (rc == 0) {
			printf("File successfully encrypted! \n");
		}
		else {
			printf("File encryption failed! \n");
		}

    } else {
        rc = myencrypt(0, filepath, argv[3]);   // 0 for decryption
		if (rc == 0) {
			printf("File successfully decrypted! \n");
		}
		else {
			printf("File decryption failed! \n");
		}
    }

    return 0;
}
