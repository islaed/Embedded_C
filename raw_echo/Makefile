SERVER = server
CLIENT = client
OBJECT = client_list.o


all: $(SERVER) $(CLIENT)


$(SERVER): server.c $(OBJECT)
	gcc server.c $(OBJECT) -o $(SERVER)

$(OBJECT): client_list.c
	gcc -c client_list.c -o $(OBJECT)

$(CLIENT): client.c
	gcc client.c -o $(CLIENT)

clean:
	rm -rf $(SERVER) $(CLIENT) $(OBJECT)