SERVER = server
CLIENT = client

all: $(SERVER) $(CLIENT)

$(SERVER): server.c
	gcc server.c -lrt -lpthread -o $(SERVER)

$(CLIENT): client.c
	gcc client.c -lrt -lpthread -o $(CLIENT)

clean:
	rm -rf $(SERVER) $(CLIENT)