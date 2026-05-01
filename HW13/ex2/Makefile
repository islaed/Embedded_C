SERVER = server
CLIENT = client
OBJECT = dlist.o

all: $(SERVER) $(CLIENT)

$(SERVER): server.c  $(OBJECT)
	gcc server.c $(OBJECT) -o $(SERVER)

$(CLIENT): client.c $(OBJECT)
	gcc client.c $(OBJECT) -o $(CLIENT)

$(OBJECT): dlist.c
	gcc -c dlist.c -o $(OBJECT)

clean:
	rm -rf $(SERVER) $(CLIENT) $(OBJECT)