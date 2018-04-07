// Modified 1.2
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <lib.h>
#include <errno.h>
#include <sys/cdefs.h>
#include <sys/types.h>
#include <sys/wait.h>


int myencrypt(int mode, const char* name, const char *password)
{
   printf("myencrypt()\n");
   message m;
   printf("Initialize message\n");
   m.m7_i1 = mode;
   m.m7_p1 = (char*) name;
   m.m7_p2 = (char*) password;
   printf("Initialize message 2\n");
   m.m7_i2 = strlen(name) + 1;
   printf("Initialize message 3\n");
   m.m7_i3 = strlen(password) + 1;
   printf("pre-systemcall\n");
    //calling MYENCRYPT syscall
    printf("name: %s\n", m.m7_p1);
    printf("password: %s\n", m.m7_p2);
    printf("myencrypt return: %i\n", VFS_PROC_NR);
    printf("myencrypt return: %i\n", VFS_MYENCRYPT);
    printf("myencrypt return: %i\n", _syscall(VFS_PROC_NR, VFS_ENCRYPT, &m));


         return 0;
}
