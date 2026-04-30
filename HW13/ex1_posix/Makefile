SERVER = server
CLIENT = client

all: $(SERVER) $(CLIENT)

$(SERVER): server.c
	gcc server.c -lrt -o $(SERVER)

$(CLIENT): client.c
	gcc client.c -lrt -o $(CLIENT)

clean:
	rm -rf $(SERVER) $(CLIENT)