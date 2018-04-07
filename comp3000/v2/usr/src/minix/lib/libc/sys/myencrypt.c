// Modified 2.0
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/cdefs.h>
#include "namespace.h"
#include <lib.h>
#include <string.h>
#include <myencrypt.h>

int myencrypt(int mode, char *name, char *password)
{
    // initialize message
    message m;
    m.m7_i1 = mode;
    m.m7_p1 = (char*) name;
    m.m7_p2 = (char*) password;
    m.m7_i2 = strlen(name) + 1;
    m.m7_i3 = strlen(password) + 1;

    // calling MYENCRYPT syscall
    int rc;
    rc = _syscall(VFS_PROC_NR, VFS_MYENCRYPT, &m);

    return rc;
}
