#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include "definitions.h"

int signup(int option, char* username, char* password){
    //create file of filename - username.txt
    char filename[BUFSIZE];
    strcpy(filename, username);
    char extension[5] = ".txt";
    strncat(filename, extension, sizeof(extension));

    //check user doesn't exist
    int fd = open(filename, O_WRONLY);
    if(fd != -1) 
        return -1;
    close(fd);

    fd = open(filename, O_WRONLY|O_CREAT, 0644);
    if(fd == -1){
        perror("signup"); 
        return -1;
    }
    struct user u;
    strcpy(u.username, username);
    strcpy(u.password, password);
    switch(option){
        case SIGNUPUSER :
            strcpy(u.type, "normal");
            break;
        case ADDUSER :
            strcpy(u.type, "normal");
            break;
        case SIGNUPJOINT :
            strcpy(u.type, "joint");
            break;
        case SIGNUPADMIN :
            strcpy(u.type, "admin");
            break;
    }
    write(fd, &u, sizeof(struct user));

    struct account acc;
    acc.balance = 0;
    write(fd, &acc, sizeof(struct account));
    
    close(fd);
    return 0;
}

int signin(int option, char* username, char* password){
    static struct flock lock;
    lock.l_type = F_RDLCK;
    lock.l_start = 0;
    lock.l_whence = SEEK_SET;
    lock.l_len = sizeof(struct user);
    lock.l_pid = getpid();

    char filename[BUFSIZE];
    strcpy(filename,username);
    char extension[5] = ".txt";
    int fd;
    strncat(filename, extension, sizeof(extension));
    fd = open(filename, O_RDONLY, 0644);
    if(fd == -1){
        perror("signin"); 
        return -1;
    }
    struct user u;
    if(fcntl(fd, F_SETLKW, &lock) == -1) {
        perror("fcntl"); 
        return -1;
    }
    
    // start of critical section
    lseek(fd, 0, SEEK_SET);
    read(fd, &u, sizeof(struct user));
    if((strcmp(u.password, password) != 0) 
        || (option == SIGNINUSER && (strcmp(u.type, "normal") != 0))
        || (option == SIGNINADMIN && (strcmp(u.type, "admin") != 0))
        || (option == SIGNINJOINT && (strcmp(u.type, "joint") != 0))) 
        return -1;
    // end of critical section
    
    lock.l_type = F_UNLCK;
    fcntl(fd, F_SETLKW, &lock);

    close(fd);
    return 0;
}
//convert balance, withdraw and deposit into 1 function
int deposit(char* username, int amt){
    static struct flock lock;
    lock.l_type = F_WRLCK;
    lock.l_start = sizeof(struct user);
    lock.l_whence = SEEK_SET;
    lock.l_len = sizeof(struct account);
    lock.l_pid = getpid();
    
    char filename[BUFSIZE];
    strcpy(filename, username);
    char extension[5] = ".txt";
    int fd;
    strncat(filename, extension, sizeof(extension));
    fd = open(filename, O_RDWR, 0644);
    if(fd == -1){
        perror("signin"); return -1;
    }

    struct account acc;
    if(fcntl(fd, F_SETLKW, &lock)==-1) {
        perror("fcntl") ;
        return -1;
    }
    
    // start of critical section
    lseek(fd, sizeof(struct user), SEEK_SET);
    if(read(fd,&acc,sizeof(struct account)) == -1) {
        perror("read"); 
        return -1;
    }
    acc.balance += amt;
    lseek(fd, sizeof(struct user), SEEK_SET);
    if(write(fd, &acc, sizeof(struct account)) == -1) {
        perror("write"); 
        return -1;
    }
    // end of critical section

    lock.l_type = F_UNLCK;
    fcntl(fd, F_SETLKW, &lock);
    close(fd);
    return 0;
}

int withdraw(char* username, int amt){
    static struct flock lock;
    lock.l_type = F_WRLCK;
    lock.l_start = sizeof(struct user);
    lock.l_whence = SEEK_SET;
    lock.l_len = sizeof(struct account);
    lock.l_pid = getpid();

    char filename[BUFSIZE];
    strcpy(filename,username);
    char extension[5] = ".txt";
    int fd;
    strncat(filename,extension,sizeof(extension));
    fd = open(filename,O_RDWR,0644);
    if(fd == -1){
        perror("signin"); return -1;
    }

    struct account acc;
    if(fcntl(fd, F_SETLKW, &lock)==-1) {
        perror("fcntl") ; 
        return -1;
    }

    // start of critical section
    lseek(fd,sizeof(struct user),SEEK_SET);
    if(read(fd,&acc,sizeof(struct account)) == -1) {perror("read"); return -1;}
    printf("balance = %d\n",acc.balance);
    acc.balance -= amt;
    if(acc.balance < 0) return -1;
    lseek(fd,sizeof(struct user),SEEK_SET);
    if(write(fd,&acc,sizeof(struct account))==-1) {perror("write"); return -1;}
    // end of critical section
    lock.l_type = F_UNLCK;
    fcntl(fd,F_SETLKW,&lock);
    close(fd);
    return 0;
}

int balance(char* username){
    static struct flock lock;
    lock.l_type = F_RDLCK;
    lock.l_start = sizeof(struct user);
    lock.l_whence = SEEK_SET;
    lock.l_len = sizeof(struct account);
    lock.l_pid = getpid();

    char filename[BUFSIZE];
    strcpy(filename,username);
    char extension[5] = ".txt";
    int fd;
    strncat(filename,extension,sizeof(extension));
    fd = open(filename,O_RDONLY,0644);
    if(fd == -1){
        perror("signin"); return -1;
    }
    struct account acc;
    if(fcntl(fd, F_SETLKW, &lock)==-1) {perror("fcntl") ; return -1;}
    // start of critical section
    lseek(fd,sizeof(struct user),SEEK_SET);
    if(read(fd,&acc,sizeof(struct account))==-1) perror("read");
    // end of critical section
    lock.l_type = F_UNLCK;
    fcntl(fd,F_SETLKW,&lock);
    close(fd);
    return acc.balance;
}

int change_password(char* username, char* pwd){
    static struct flock lock;
    lock.l_type = F_WRLCK;
    lock.l_start = 0;
    lock.l_whence = SEEK_SET;
    lock.l_len = sizeof(struct user);
    lock.l_pid = getpid();

    char filename[BUFSIZE];
    strcpy(filename,username);
    char extension[5] = ".txt";
    int fd;
    strncat(filename,extension,sizeof(extension));
    fd = open(filename,O_RDWR,0644);
    if(fd == -1){
        perror("change pwd"); 
        return -1;
    }
    struct user u;
    lseek(fd,0,SEEK_SET);
    if(fcntl(fd, F_SETLKW, &lock)==-1) {
        perror("fcntl") ;
        return -1;
    }

    // start of critical section
    if(read(fd,&u,sizeof(struct user))==-1) { 
        perror("read"); 
        return -1; 
    }
    strcpy(u.password,pwd);
    lseek(fd, 0, SEEK_SET);
    if(write(fd, &u, sizeof(struct user))==-1) { 
        perror("write"); 
        return -1; 
    }
    // end of critical section

    lock.l_type = F_UNLCK;
    fcntl(fd, F_SETLKW, &lock);
    close(fd);
    return 0;
}

char* get_details(char* username){
    static struct flock lock;
    lock.l_type = F_RDLCK;
    lock.l_start = 0;
    lock.l_whence = SEEK_SET;
    lock.l_len = 0;
    lock.l_pid = getpid();

    char filename[BUFSIZE];
    strcpy(filename, username);
    char extension[5] = ".txt";
    int fd;
    strncat(filename, extension, sizeof(extension));
    fd = open(filename, O_RDWR, 0644);
    if(fd == -1){
        perror("open"); 
        return "user does not exist\n";
    }
    struct account acc;
    struct user u;
    if(fcntl(fd, F_SETLKW, &lock)==-1) {
        perror("fcntl"); 
        return "sorry, section is locked\n";
    }

    // start of critical section
    lseek(fd,0,SEEK_SET);
    if(read(fd,&u,sizeof(struct user))==-1){
        perror("read"); 
        return "unable to read file\n";
        }
    if(read(fd,&acc,sizeof(struct account))==-1){
        perror("read"); 
        return "unable to read file\n";
    }
    // end of critical section

    lock.l_type = F_UNLCK;
    fcntl(fd,F_SETLKW,&lock);
    close(fd);
    char* return_string = (char*)malloc(BUFSIZE * sizeof(char));
    sprintf(return_string,"username : %s \npassword : %s \ntype : %s\nbalance : %d\n",
        u.username,u.password,u.type,acc.balance);
    return return_string;
}