typedef struct
{
    dev_t dev; //device no
    ino_t inode;
    char hash_pw[65];
}tableEntry;

#define BASE_CHAR 0
#define MAX_CHAR 255
#define MAX_KEY_LENGTH 65
#define ENCRYPTED_FILE_TABLE "/etc/encryptedFiles"
#define ENCRYPTED_FILE_TABLE_KEY "password"
#define INVALID_INODE -1 //Used for deleting entries from the table
#define BUFFER_SIZE 256

tableEntry node1, node2;

//unsigned long mybuffer;


char BUFFER[BUFFER_SIZE];
