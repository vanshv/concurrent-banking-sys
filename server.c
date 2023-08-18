#include <unistd.h> 
#include <stdio.h>
#include <stdlib.h> 
#include <netinet/in.h> 
#include <string.h> 
#include <pthread.h>
#include <sys/socket.h> 
#include <sys/types.h>
#include "definitions.h"

extern int signup(int, char*, char*); 
extern int signin(int, char*, char*);
extern int deposit(char*, int);
extern int withdraw(char*, int);
extern int balance(char*);
extern int change_password(char*, char*);
extern char* get_details(char*);
void *connection_handler(void *);
extern int modify_user (char*, char*, char*);
extern int del_user(char*);

int main(int argc, char const *argv[]) 
{ 
	int server_fd, new_socket, valread; 
	struct sockaddr_in address; 
	int opt = 1; 
	int addrlen = sizeof(address); 
	if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) { 
		perror("socket failed"); 
		exit(EXIT_FAILURE); 
	} 

	if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) { 
		perror("setsockopt"); 
		exit(EXIT_FAILURE); 
	} 
	address.sin_family = AF_INET; 
	address.sin_addr.s_addr = INADDR_ANY; 
	address.sin_port = htons( PORT ); 
	
	// Attaching socket to the port 8080 
	if (bind(server_fd, (struct sockaddr *)&address, sizeof(address))<0) { 
		perror("bind failed"); 
		exit(EXIT_FAILURE); 
	} 
	if (listen(server_fd, 3) < 0) { 
		perror("listen"); 
		exit(EXIT_FAILURE); 
	} 
    printf("ready to listen!!\n");
    while(1){
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen))<0) { 
            perror("accept"); 
            exit(EXIT_FAILURE); 
        } 
        pthread_t thread_id;
        if( pthread_create( &thread_id , NULL ,  connection_handler , (void*) &new_socket) < 0)
        {
            perror("could not create thread");
            return 1;
        }
        puts("Handler assigned");
    }
	return 0; 
} 

void *connection_handler(void *socket_desc)
{
	int sock = *(int*)socket_desc , option, deposit_amt, withdraw_amt, ret,balance_amt;
	char* username = malloc(BUFSIZE*sizeof(char));
	char* password = malloc(BUFSIZE*sizeof(char));
	while(1){
		char* type = malloc(BUFSIZE*sizeof(char));
		char* new_username = malloc(BUFSIZE*sizeof(char));
		char* return_message = malloc(BUFSIZE*sizeof(char));
		char* option_string = malloc(BUFSIZE*sizeof(char));
		char* amt_string = malloc(BUFSIZE*sizeof(char));
		printf("sock = %d\n",sock);
		read( sock , option_string, sizeof(option_string)); 
		option = atoi(option_string);
		if (option == SIGNUPUSER || option == SIGNUPADMIN || option == SIGNUPJOINT){
			read(sock, username, sizeof(username));
			read(sock, password, sizeof(password));
			int ret = signup(option,username,password);
			if(ret == -1) 
				return_message = "User could not be added\n";
			else 
				return_message = "User added successfully!\n";
		}
		else if (option == SIGNINUSER || option == SIGNINADMIN || option == SIGNINJOINT){
			read(sock ,username, sizeof(username));
			read(sock ,password, sizeof(password));
			ret = signin(option,username,password);
			if(ret == -1) 
				return_message = "sign in failed\n";
			else 
				return_message = "successfully signed in!\n";
		}
		else if (option == DEPOSIT) {
			read(sock,amt_string,sizeof(amt_string));
			deposit_amt = atoi(amt_string);
			ret = deposit(username,deposit_amt);
			if (ret == 0) 
				return_message = "amount deposited\n";
		}
		else if (option == WITHDRAW) {
			read(sock,amt_string,sizeof(amt_string));
			withdraw_amt = atoi(amt_string);
			ret = withdraw(username,withdraw_amt);
			if (ret == -1) 
				return_message = "unable to withdraw\n";
			else 
				return_message = "withdrew successfully\n";
		}
		else if (option == BALANCE) {
			balance_amt = balance(username);
			sprintf(return_message,"%d",balance_amt);
		}
		else if (option == PASSWORD) {
			read(sock, password, sizeof(password));
			ret = change_password (username, password);
			if (ret == -1) 
				return_message = "unable to change password\n";
			else 
				return_message = "changed password successfully\n";
		}
		else if (option == DETAILS) {
			return_message = get_details(username);
		}
		else if(option == DELUSER) {
			char* username = malloc(BUFSIZE*sizeof(char));
			char* password = malloc(BUFSIZE*sizeof(char));
			read(sock, username, sizeof(username));
			ret = del_user(username);
			printf("unlink returned %d\n",ret);
			if (ret == -1) 
				return_message = "unable to delete user\n";
			else 	
				return_message = "user deleted successfully\n";
		}
		else if(option == MODUSER) {
			char* username = malloc(BUFSIZE*sizeof(char));
			char* password = malloc(BUFSIZE*sizeof(char));

			read(sock, username, sizeof(username));
			read(sock, new_username, sizeof(new_username));
			read(sock, password, sizeof(password));

			ret = modify_user (username, new_username, password);
			if (ret == -1) 
				return_message = "unable to change user\n";
			else 
				return_message = "changed user successfully\n";
		}
		else if (option == GETUSERDETAILS) {
			char* username = malloc(BUFSIZE*sizeof(char));
			read(sock, username, sizeof(username));
			printf("username = %s\n", username);
			return_message = get_details(username);
		}
		else if (option == ADDUSER) {
			char* username = malloc(BUFSIZE*sizeof(char));
			char* password = malloc(BUFSIZE*sizeof(char));
			read(sock, type, sizeof(type));
			read(sock, username, sizeof(username));
			read(sock, password, sizeof(password));
			printf("type = %s username = %s pwd = %s\n", type, username, password);
			if(!strcmp(type,"1")) 
				option = SIGNUPUSER;
			else 
				option = SIGNUPJOINT;
			ret = signup(option, username, password);
			if(ret == -1) 
				return_message = "account could not be added\n";
			else 
				return_message = "successfully added account!\n";
		}
		send(sock, return_message, BUFSIZE * sizeof(char) , 0 ); 
	}
    return 0;
} 
