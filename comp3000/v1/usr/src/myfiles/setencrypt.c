//Modified 1.0
#include <stdio.h>
#include "myencrypt.h"

int main(int argc, char **argv)
{
    char path[256];

    getcwd(path, sizeof(path));
    char buffer[256];
    snprintf(buffer, sizeof buffer, "%s/%s", path, argv[2]);


    printf("LALALALALA\n");
    printf(" full path 1.0\n");
    printf("%s\n", buffer);
    printf("problem with buffer\n");

    printf("promblem with strcomp: %i\n", strcmp(argv[1],"-e"));

    if (strcmp(argv[1],"-e") == 0) {
        printf("Test0 \n");
        printf("argv[3]: %s\n", argv[3]);
        myencrypt(1, buffer, argv[3]);   // 1 for encryption
        printf("Ran myencrypt\n");
    } else {
        printf("Test1");
        myencrypt(0, buffer, argv[3]);   // 0 for decryption
    }

    return 0;
}
