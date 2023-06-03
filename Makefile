TARGET = client_multi_os server_th_window

all: $(TARGET)

client_multi_os: client_multi_os.c
	gcc -o $@ client_multi_os.c

server_th_window: server_th_window.c
	gcc -o $@ server_th_window.c -pthread

clean:
	rm -f $(TARGET)
