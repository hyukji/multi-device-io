#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <linux/input.h>

#include <sys/socket.h>


// it is just experiment for getting input_event
int main(int argc, char* argv[]) {
    struct input_event event;
    struct timeval tv;

    int fd = open(argv[1], O_RDONLY);

    if(fd < 0) {
        perror("error in open event file");
        return 1;
    }
    
    int result_fd = open("inputResult.txt", O_WRONLY | O_TRUNC);
    if(result_fd < 0) {
        perror("error in open event file");
        return 1;
    }
    
    while (1) {
        if(read(fd, &event, sizeof(event)) == -1) {
            perror("error in read event");
            close(fd);
            break;
        } else {
	    tv = event.time;
	    
	    char buffer[128];
	    sprintf(buffer, "time %ld %ld, type %d, code %d, value %d\n", tv.tv_sec, tv.tv_usec,  event.type, event.code, event.value);
    
            printf("%s", buffer);
            
            write(result_fd, buffer, strlen(buffer));
        }
    }

    close(result_fd);
    close(fd);
    return 0;
}
