#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include "definitions.h"

extern int del_user(char*);
extern int signup(int, char*, char*);

int del_user(char* username){
    static struct flock lock;
    lock.l_type = F_WRLCK;
    lock.l_start = 0;
    lock.l_whence = SEEK_SET;
    lock.l_len = 0;
    lock.l_pid = getpid();

    char filename[BUFSIZE];
    strcpy(filename, username);
    char extension[5] = ".txt";
    strncat(filename, extension, sizeof(extension));
    int fd = open(filename,O_RDWR,0644);
    if(fd == -1)
        perror("open");

    if(fcntl(fd, F_SETLKW, &lock) == -1) 
        perror("fcntl");

    return unlink(filename);
}

int modify_user(char* username, char* new_username, char* password) {
    static struct flock lock;
    lock.l_type = F_WRLCK;
    lock.l_start = 0;
    lock.l_whence = SEEK_SET;
    lock.l_len = 0;
    lock.l_pid = getpid();

    char filename[BUFSIZE];
    strcpy(filename,username);
    char extension[5] = ".txt";
    int fd,option;
    strncat(filename,extension,sizeof(extension));
    fd = open(filename,O_RDWR,0644);
    if(fd == -1)
        perror("mod user"); return -1;
    if(fcntl(fd, F_SETLKW, &lock)==-1) 
        perror("fcntl");

    struct user usr;
    // start of critical section
    lseek(fd,0,SEEK_SET);
    if(read(fd, &usr, sizeof(struct user)) == -1){
        perror("read"); 
        return -1; 
    }
    //files are deleted since username defines filename
    del_user(username);

    if(strcmp(usr.type, "normal") == 0) 
        option = SIGNUPUSER;
    else 
        option = SIGNUPJOINT;
    strcpy(usr.username, new_username);
    signup(option, new_username, password);
    // end of critical section
    lock.l_type = F_UNLCK;
    fcntl(fd, F_SETLKW, &lock);
    close(fd);
    return 0;
}