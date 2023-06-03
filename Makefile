TARGET = client server_with_sensor server_without_sensor

all: $(TARGET)

client: client.c
	gcc -o $@ client.c
	
server_without_sensor: server_without_sensor.c
	gcc -o $@ server_without_sensor.c -pthread

server_with_sensor: server_with_sensor.c
	gcc -o $@ server_with_sensor.c -pthread -lwiringPi

clean:
	rm -f $(TARGET)
