TARGET = server client input

all: $(TARGET)

server: server.c
	gcc -o $@ server.c

client: client.c
	gcc -o $@ client.c

input: input.c
	gcc -o $@ input.c

clean:
	rm -f $(TARGET)


