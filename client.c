#include <stdio.h> 
#include <sys/socket.h> 
#include <arpa/inet.h> 
#include <unistd.h> 
#include <string.h> 
#include <stdlib.h>
#include "definitions.h"

int printOptions(int);
void interface(int);
int get_auth_details(int, int);
void user_interface(int);
void admin_interface();

int main(int argc, char const *argv[]) 
{ 
	int sock = 0, valread; 
	struct sockaddr_in serv_addr; 
	char *hello = "Hello from client"; 
	char buffer[1024] = {0}; 
	if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0){ 
		printf("\n Socket creation error \n"); 
		return -1; 
	} 

	serv_addr.sin_family = AF_INET; 
	serv_addr.sin_port = htons(PORT); 

	if(inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr )<= 0){
		printf("\nInvalid address/ Address not supported \n"); 
		return -1; 
	}

	if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0){ 
		printf("\nConnection Failed \n"); 
		return -1; 
	} 
	interface(sock);
	return 0; 
} 

void interface(int sock){
	int option, opt_ret;
	char* option_string = malloc(10*sizeof(char));
	char* return_message = malloc(BUFSIZE*sizeof(char));
	option = printOptions(STARTMENU);
	switch (option) {
		case SIGNUP:
			opt_ret = get_auth_details(sock, SIGNUPMENU);
			while(1) {
				switch (opt_ret) {
					case SIGNUPUSER : 
						user_interface(sock);
						break;
					case SIGNUPJOINT :
						user_interface(sock);
						break;
					case SIGNUPADMIN :
						admin_interface(sock);
						break;
				}
			}
			break;
		case SIGNIN:
			opt_ret = get_auth_details(sock, SIGNINMENU);
			switch (opt_ret) {
				case SIGNINUSER :
					while(1) {
						user_interface(sock);
					}
					break;
				case SIGNINADMIN :
					while(1) {
						admin_interface(sock);
					}
					break;
				case SIGNINJOINT :
					while(1) {
						user_interface(sock);
					}
			}
			break;
		default:
			printf("Invalid Option\n");
			exit(1);
	}
	
}

//what does this even do?
int get_auth_details(int sock, int option){
	char* username = malloc(BUFSIZE*sizeof(char));
	char* password = malloc(BUFSIZE*sizeof(char));
	char* return_message = malloc(BUFSIZE*sizeof(char));
	char* option_string = malloc(10*sizeof(char));
	
	printf("option = %d\n",option);
	int ret_option = printOptions(option);
	printf("Enter username : ");
	scanf("%s",username);
	printf("Enter password : ");
	scanf("%s",password);
	sprintf(option_string,"%d",ret_option);
	send(sock, option_string, sizeof(option_string), 0 ); 
	send(sock, username, sizeof(username), 0 ); 
	send(sock, password, sizeof(password), 0 ); 
	read(sock, return_message, BUFSIZE * sizeof(char)); 
	printf("%s\n",return_message); 
	if(!strcmp(return_message,"sign in failed\n")) exit(1);
	return ret_option;
}

void user_interface(int sock){
	int option = printOptions(USERMENU), deposit_amt, withdraw_amt;
	char* option_string = malloc(10*sizeof(char));
	char* return_message = malloc(BUFSIZE*sizeof(char));
	char* amt_string = malloc(BUFSIZE*sizeof(char));
	char* password = malloc(BUFSIZE*sizeof(char));
	sprintf(option_string,"%d",option);
	send(sock, option_string, sizeof(option_string), 0 ); 
	switch (option) {
		case DEPOSIT : 
			printf("Enter amount to be deposited : ");
			scanf("%d",&deposit_amt);
			sprintf(amt_string,"%d",deposit_amt);	
			printf("%s\n",amt_string);		
			send(sock, amt_string, sizeof(amt_string), 0);
			break;
		case WITHDRAW :
			printf("Enter amount to be withdrawn : ");
			scanf("%d",&withdraw_amt);
			sprintf(amt_string,"%d",withdraw_amt);
			send(sock, amt_string, sizeof(amt_string), 0);
			break;
		case BALANCE : 
			break;
		case PASSWORD : 
			printf("Enter new password\n");
			scanf("%s",password);
			send(sock, password, sizeof(password), 0);
			break;
		case DETAILS : 
			break;
		case EXIT : 
			exit(0);
	}
	read(sock, return_message, BUFSIZE * sizeof(char)); 
	printf("%s\n",return_message); 
}

void admin_interface(int sock){
	int option = printOptions(ADMINMENU), type;
	char* option_string = malloc(10*sizeof(char));
	char* username = malloc(BUFSIZE*sizeof(char));
	char* new_username = malloc(BUFSIZE*sizeof(char));
	char* password = malloc(BUFSIZE*sizeof(char));
	char* return_message = malloc(BUFSIZE*sizeof(char));
	sprintf(option_string,"%d",option);
	send(sock , option_string , sizeof(option_string) , 0 ); 
	switch (option) {
		case ADDUSER :
			printf("Enter User Type\n");
			printf("1 : Normal\n");
			printf("2 : Joint\n");
			scanf("%d",&type);
			printf("Enter username : ");
			scanf("%s",username);
			printf("Enter password : ");
			scanf("%s",password);
			switch (type) {
				case 1 : 
					send(sock , "1" , sizeof("1") , 0 ); 
					break;
				case 2 : 
					send(sock , "2" , sizeof("2") , 0 ); 
					break;
				default :
					printf("Invalid Type\n");
					exit(1);
			}
			printf("username = %s\n",username);
			printf("password = %s\n",password);
			send(sock , username , sizeof(username) , 0 ); 
			send(sock , password , sizeof(password) , 0 ); 
			break;
		case DELUSER : 
			printf("Enter username : ");
			scanf("%s",username);
			send(sock , username , sizeof(username) , 0 ); 
			break;
		case MODUSER : 
			printf("Enter old username : " );
			scanf("%s",username);
			send(sock , username , sizeof(username) , 0 ); 
			printf("Enter new username : ");
			scanf("%s",new_username);
			send(sock , new_username , sizeof(new_username) , 0 ); 
			printf("Enter new password : ");
			scanf("%s",password);
			send(sock , password , sizeof(password) , 0 ); 
			break;
		case GETUSERDETAILS : 
			printf("Enter username : ");
			scanf("%s",username);
			send(sock , username , sizeof(username) , 0 ); 
			break;
		case EXIT : 
			exit(0);
		default :
			printf("Invalid input\n");
			exit(0);
	}
	read(sock, return_message, BUFSIZE * sizeof(char)); 
	printf("%s\n",return_message); 
}

int printOptions(int menu){
	int option;
	switch (menu) {
		case STARTMENU:
			printf("Welcome, Please enter one of the following options -\n");
			printf("1 : Sign Up\n");
			printf("2 : Sign In\n");
			scanf("%d",&option);
			switch(option) {
				case 1 :
					return SIGNUP;
				case 2 :
					return SIGNIN;
				default : 
					return INVALID ;
			} 
		case SIGNUPMENU:
			printf("Whom would you like to sign up as?\n");
			printf("1 : Simple User\n");
			printf("2 : Joint Account User\n");
			printf("3 : Administrator\n");
			scanf("%d",&option);
			switch(option) {
				case 1 :
					return SIGNUPUSER;
				case 2 :
					return SIGNUPJOINT;
				case 3 : 
					return SIGNUPADMIN;
				default :
					return INVALID;
			}
		case SIGNINMENU : 
			printf("Whom would you like to sign in as?\n");
			printf("1 : Simple User\n");
			printf("2 : Joint Account User\n");
			printf("3 : Administrator\n");
			scanf("%d",&option);
			switch(option) {
				case 1 :
					return SIGNINUSER;
				case 2 :
					return SIGNINJOINT;
				case 3 : 
					return SIGNINADMIN;
				default :
					return -1;
			}
		case USERMENU : 
			printf("What would you like to do?\n");
			printf("1 : Deposit\n");
			printf("2 : Withdraw\n");
			printf("3 : Check Balance\n");
			printf("4 : Change Password\n");
			printf("5 : View Details\n");
			printf("6 : Exit\n");
			scanf("%d",&option);
			switch(option) {
				case 1 : return DEPOSIT;
				case 2 : return WITHDRAW;
				case 3 : return BALANCE;
				case 4 : return PASSWORD;
				case 5 : return DETAILS;
				case 6 : return EXIT;
				default : return -1;
			}
		case ADMINMENU :
			printf("1 : Add User\n");
			printf("2 : Delete User\n");
			printf("3 : Modify User\n");
			printf("4 : Search for Account Details\n");
			printf("5 : Exit\n") ;
			scanf("%d",&option);
			switch (option) {
				case 1 : return ADDUSER;
				case 2 : return DELUSER;
				case 3 : return MODUSER;
				case 4 : return GETUSERDETAILS;
				case 5 : return EXIT;
				default : return -1;
			}
		default:
			break;
	}
}
