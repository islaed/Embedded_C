SERVER = server
TCP_CLIENT = tcp_client
UDP_CLIENT = udp_client

all: $(SERVER) $(TCP_CLIENT) $(UDP_CLIENT)

$(SERVER): server.c
	gcc server.c -o $(SERVER)

$(TCP_CLIENT): tcp_client.c
	gcc tcp_client.c -o $(TCP_CLIENT)

$(UDP_CLIENT): udp_client.c
	gcc udp_client.c -o $(UDP_CLIENT)

clean:
	rm -rf $(SERVER) $(TCP_CLIENT) $(UDP_CLIENT)
