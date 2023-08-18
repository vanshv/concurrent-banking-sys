#define PORT 8080 

// possible menus
#define STARTMENU 1
#define SIGNUPMENU 2
#define SIGNINMENU 3
#define USERMENU 4
#define ADMINMENU 5

// possible options
#define SIGNUP 5
#define SIGNIN 6
#define SIGNUPUSER 7
#define SIGNUPJOINT 8
#define SIGNUPADMIN 9
#define SIGNINUSER 10
#define SIGNINJOINT 11
#define SIGNINADMIN 12
#define DEPOSIT 13
#define WITHDRAW 14
#define BALANCE 15
#define PASSWORD 16
#define DETAILS 17
#define EXIT 18
#define ADDUSER 19
#define DELUSER 20
#define MODUSER 21
#define GETUSERDETAILS 22
#define INVALID -1
#define BUFSIZE 1000

//struct user
struct user {
    char type[BUFSIZE];
    char password[BUFSIZE];
    char username[BUFSIZE];
};

//struct details
struct account {
    int balance;
};



