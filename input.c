#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <linux/input.h>

#include <sys/socket.h>


// it is just experiment for getting input_event
int main() {
    struct input_event event;
    struct timeval tv;

    int fd = open("/dev/input/event2", O_RDONLY);
    if(fd < 0) {
        perror("error in open event file");
        return 1;
    }

    while (1) {
        if(read(fd, &event, sizeof(event)) == -1) {
            perror("error in read event");
            close(fd);
            return 1;
        } else {
	    tv = event.time;
            printf("time %ld %ld, type %d, code %d, value %d\n", tv.tv_sec, tv.tv_usec,  event.type, event.code, event.value);
        }
    }

    close(fd);
    return 0;
}
