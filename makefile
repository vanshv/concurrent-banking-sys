server:
	gcc server.c user_menu.c admin_menu.c -lpthread -o server
 
client:
	gcc client.c -o client

clean:
	rm client server
