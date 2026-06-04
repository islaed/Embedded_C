SERVER = server
CLIENT = client

all: $(SERVER) $(CLIENT)

$(SERVER): server.c
	gcc server.c -o server

$(CLIENT): client.c
	gcc client.c -o client

clean:
	rm -rf $(SERVER) $(CLIENT)