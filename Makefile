TARGET = server client input client_char server_th_window

all: $(TARGET)

server: server.c
	gcc -o $@ server.c

client: client.c
	gcc -o $@ client.c

input: input.c
	gcc -o $@ input.c

client_char: client_char.c
	gcc -o $@ client_char.c

server_th_window: server_th_window.c
	gcc -o $@ server_th_window.c -pthread


clean:
	rm -f $(TARGET)


