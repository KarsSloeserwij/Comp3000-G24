typedef struct
{
    dev_t dev;
    ino_t inode;
    char  pwd[256];
}tableEntry;

#define MAX_CHAR 255
#define ENCRYPTED_FILE_TABLE "/etc/encryptedFiles"
