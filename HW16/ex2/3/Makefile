SERVER = server
CLIENT = client

all: $(SERVER) $(CLIENT)

$(SERVER): server.c queue.c queue.h
	gcc server.c queue.c -o $(SERVER) -lpthread

$(CLIENT): client.c
	gcc client.c -o $(CLIENT)

clean:
	rm -rf $(SERVER) $(CLIENT)