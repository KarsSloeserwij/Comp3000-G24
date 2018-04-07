#include "fs.h"
#include <sys/stat.h>
#include <unistd.h>
#include <assert.h>
#include <minix/callnr.h>
#include "file.h"
#include "path.h"
#include <minix/vfsif.h>
#include "vnode.h"
#include "vmnt.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "password.h"
#include <fcntl.h>


int writeToTable(tableEntry *node)
{
    char *filePath = ENCRYPTED_FILE_TABLE;
    struct lookup resolve;
    struct vmnt *vmp;
    struct vnode *vp;
    int request;
    u64_t new_pos;
    unsigned int iop;


    lookup_init(&resolve, filePath, PATH_NOFLAGS, &vmp, &vp);
    resolve.l_vmnt_lock = VMNT_WRITE;
    resolve.l_vnode_lock = VNODE_WRITE;

    if((vp = eat_path(&resolve, fp)) == NULL)
    {
        return(err_code);
    }

    request = req_readwrite(vp->v_fs_e, vp->v_inode_nr,vp->v_size, WRITING, fproc[VFS_PROC_NR].fp_endpoint,  (vir_bytes)node, sizeof(tableEntry), &new_pos, &iop);

    unlock_vnode(vp);
    if(vmp != NULL)
    {
      unlock_vmnt(vmp);
    }
    put_vnode(vp);

    return OK;
}

int removeFromTable(u64_t position)
{
    char *fullpath = ENCRYPTED_FILE_TABLE;
    struct lookup resolve;
    struct vmnt *vmp;
    struct vnode *vp;
    int request;
    u64_t new_pos;
    unsigned int iop;

    lookup_init(&resolve, fullpath, PATH_NOFLAGS, &vmp, &vp);
    resolve.l_vmnt_lock = VMNT_WRITE;
    resolve.l_vnode_lock = VNODE_WRITE;

    if((vp = eat_path(&resolve, fp)) == NULL)
    {
        return(err_code);
    }

    tableEntry emptyNode;
    emptyNode.inode = -1;
    emptyNode.dev = -1;
    uintptr_t address = (uintptr_t)&emptyNode;
    request = req_readwrite(vp->v_fs_e, vp->v_inode_nr, position, WRITING, fproc[VFS_PROC_NR].fp_endpoint,  address, sizeof(emptyNode), &new_pos, &iop);

    unlock_vnode(vp);
    if(vmp != NULL)
    {
      unlock_vmnt(vmp);
    }
    put_vnode(vp);

    return OK;
}

int writeToFile(char *buffer, unsigned int length, char *filePath)
{
    struct lookup resolve;
    struct vmnt *vmp;
    struct vnode *vp;
    int request;
    u64_t new_pos;
    unsigned int iop;

    lookup_init(&resolve, filePath, PATH_NOFLAGS, &vmp, &vp);
    resolve.l_vmnt_lock = VMNT_WRITE;
    resolve.l_vnode_lock = VNODE_WRITE;

    if((vp = eat_path(&resolve, fp)) == NULL)
    {
        return(err_code);
    }

    request = req_readwrite(vp->v_fs_e, vp->v_inode_nr, 0, WRITING, fproc[VFS_PROC_NR].fp_endpoint,  (vir_bytes)buffer, length, &new_pos, &iop);

    unlock_vnode(vp);
    if(vmp != NULL)
    {
      unlock_vmnt(vmp);
    }
    put_vnode(vp);

    return OK;
}

long getFileSize(const char *filePath)
{
    struct vnode *vp;
    struct vmnt *vmp;
    struct lookup resolve;
    lookup_init(&resolve, (char *)filePath, PATH_NOFLAGS, &vmp, &vp);

    resolve.l_vmnt_lock = VMNT_READ;
    resolve.l_vnode_lock = VNODE_OPCL;

    if ((vp = eat_path(&resolve, fp)) == NULL)
    {
        printf("Unable to retreive inode!\n");
    }

    long fileSize = 0;

    if (vp) {
        unlock_vnode(vp);
        unlock_vmnt(vmp);
        put_vnode(vp);
        fileSize = vp->v_size;
    }

    return fileSize;
}

int readRecord(tableEntry *write, u64_t pos, u64_t *newPos, char *filePath)
{
    int request;
    struct vnode* vp;
    struct vmnt *vmp;
    struct lookup resolve;
    unsigned int iop;

    // intialise the stuff required to read files
    lookup_init(&resolve, filePath, PATH_NOFLAGS, &vmp, &vp);
    resolve.l_vmnt_lock  = VMNT_READ;
    resolve.l_vnode_lock = VNODE_READ;

    // retrieve the vp
    if ((vp = eat_path(&resolve, fp)) == NULL){
      return err_code;
    }

    // read file record from encryptedFiles
    request = req_readwrite(vp->v_fs_e, vp->v_inode_nr, pos, READING, fproc[VFS_PROC_NR].fp_endpoint, (vir_bytes)write, sizeof(tableEntry), newPos, &iop);

    // unlock and return any resources we acquired
    unlock_vnode(vp);
    if (vmp != NULL)
    {
      unlock_vmnt(vmp);
    }
    put_vnode(vp);

    return request;
}

int readFile(char *buffer, unsigned int length, char *filePath)
{
    int request;
    struct vnode* vp;
    struct vmnt *vmp;
    struct lookup resolve;
    u64_t new_pos;
    unsigned int iop;

    // intialise the stuff required to read files
    lookup_init(&resolve, filePath, PATH_NOFLAGS, &vmp, &vp);
    resolve.l_vmnt_lock  = VMNT_READ;
    resolve.l_vnode_lock = VNODE_READ;

    // retrieve the vp
    if ((vp = eat_path(&resolve, fp)) == NULL){
      return err_code;
    }

    // read file record from encryptedFiles
    request = req_readwrite(vp->v_fs_e, vp->v_inode_nr, 0, READING, fproc[VFS_PROC_NR].fp_endpoint, (vir_bytes)buffer, length, &new_pos, &iop);

    // unlock and return any resources we acquired
    unlock_vnode(vp);
    if (vmp != NULL)
    {
      unlock_vmnt(vmp);
    }
    put_vnode(vp);

    return request;
}

int encryptFile(int mode, char *filePath)
{
    char filePath1[strlen(filePath) + 1];
    char filePath2[strlen(filePath) + 1];
    strcpy(filePath1, filePath);
    strcpy(filePath2, filePath);
    int key = 11;
    unsigned int length = getFileSize(filePath);
    /* check if opened successfully */
    if (length == 0) {
        printf("File is empty \n");
        return -1;
    }

    char *mystring = malloc((length + 1) * sizeof(char));

    if (mystring == NULL) {
        printf("Not enough memory\n");
        return -1;
    }

    if (readFile(mystring, length, filePath1) != OK)
    {
        printf("Error reading file to encrypt.\n");
        return -1;
    }

    mystring[length] = '\0';

    for (char *ch = mystring; *ch != '\0'; ch++) {
        if (mode == 1) { //encryption
            *ch =  (int) *ch + key;
    	}

    	else {  //decryption
            *ch =  (int) *ch - key;
    	}
    }

    if (writeToFile(mystring, length, filePath2) != OK)
    {
        printf("Error writing to file to encrypt.\n");
        return -1;
    }

    return 0;
}


int do_encrypt()
{
    int rc = -1;
    // get all the information from the message
    // mode, filepath, password
    int pathLen = m_in.m7_i2;
    int pwdLen = m_in.m7_i3;
    char filepath[pathLen];
    char filepath1[pathLen];
    char password[pwdLen];
    int  mode = m_in.m7_i1;

    if (sys_datacopy(who_e, (vir_bytes) m_in.m7_p1, VFS_PROC_NR, (vir_bytes) filepath, (phys_bytes) m_in.m7_i2) != OK)
    {
        printf("Failed getting file path from message\n");
        return -1;
    }
    if (sys_datacopy(who_e, (vir_bytes) m_in.m7_p2, VFS_PROC_NR, (vir_bytes) password, (phys_bytes) m_in.m7_i3) != OK)
    {
        printf("Failed getting password from message\n");
        return -1;
    }
    strcpy(filepath1, filepath);
    password[pwdLen-1] = '\0';

    // encrypt Password
    for (char *ch = password; *ch != '\0'; ch++) {
        *ch =  (int) *ch + 17;
    }

    // check whether the file exist and wheter we can read the inode of the file
    struct lookup resolve;
    struct vmnt *vmp;
    struct vnode *vp;
    tableEntry node1, node2;
    lookup_init(&resolve, (char *)filepath, PATH_NOFLAGS, &vmp, &vp);

    resolve.l_vmnt_lock = VMNT_READ;
    resolve.l_vnode_lock = VNODE_READ;

    if ((vp = eat_path(&resolve, fp)) == NULL)
    {
        printf("Unable to retreive inode!\n");
        return -1;
    }

    //store the inode information
    node1.inode = vp->v_inode_nr;
    node1.dev = vp->v_dev;
    strcpy(node1.pwd, password);

    //allow the inode to be writen to
    unlock_vnode(vp);
    if(vmp != NULL)
    {
        unlock_vmnt(vmp);
    }
    put_vnode(vp);

    //get the number of file entries within the table already
    long table_entries = getFileSize(ENCRYPTED_FILE_TABLE) / sizeof(tableEntry);

    //initialize the variables to read the encrypted files table
    u64_t currentPos = 0;
    u64_t newPos;
    int check = 0;

    //loop through the table entries and see if there is a record of the file
    for (int i = 0; i < table_entries; i++) {
        if (readRecord(&node2, currentPos, &newPos, ENCRYPTED_FILE_TABLE) != OK)
        {
          printf("Error reading encrypted file table.\n");
          return -1;
        }

        if ( (node1.dev == node2.dev) && (node1.inode == node2.inode)) {
          check = 1;
          break;
        }
        currentPos = newPos;
    }

    // encryption mode
    if (mode == 1)
    {
      if (check == 0) {
          // add file to the table of encrypted files
          if (writeToTable(&node1) == OK) {
              // encrypt file
              rc = encryptFile(mode, filepath1);
          }
      }
      else {
          printf("File already encrypted\n");
          return -1;
      }
    }
    // decryption mode
    else
    {
      if (check == 0) {
          printf("File is not encrypted\n");
          return -1;
      }
      else {
          if (strcmp(node2.pwd, password) == 0) {
              // remove file from the table of encrypted files
              if (removeFromTable(currentPos) == OK) {
                  // decrypt file
                  rc = encryptFile(mode, filepath1);
              }
          }
          else {
              printf("Wrong password\n");
              return -1;
          }
      }
    }

    return rc;
}
