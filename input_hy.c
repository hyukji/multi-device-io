#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <linux/input.h>
#include <sys/time.h>
#include <string.h>
#include <unistd.h>
#include <termios.h>


int main()
{
        int fd, ret, code;
        int i = 0;
        int j = 16; // q 16 
        char c;
        const char* evdPath = "/dev/input/event3";
        struct input_event iev[1], syn[1], stp[1];
        struct termios oldattr, newattr;

        tcgetattr(STDIN_FILENO, &oldattr);
        newattr = oldattr;
        newattr.c_lflag &= ~(ICANON | ECHO);
        newattr.c_cc[VMIN] = 1;
        newattr.c_cc[VTIME] = 0;
        iev[0].value = 1;
        iev[0].type = EV_KEY;

        syn[0].type = EV_SYN;
        syn[0].code = 0;
        syn[0].value = 0;

        stp[0].type = EV_KEY;
        stp[0].value = 0;

        fd = open(evdPath, O_RDWR);
     //   tcsetattr(STDIN_FILENO, TCSANOW, &newattr);
        if(fd < 0) {
                perror("error");
                return -1;
        }
        while(1){
                if(j <= 20){
                    iev[0].code = j;
                    stp[0].code = j;
                    write(fd, iev, sizeof(struct input_event));
                    write(fd, syn, sizeof(struct input_event));
                    write(fd, stp, sizeof(struct input_event));
                    write(fd, syn, sizeof(struct input_event));
                    usleep(100000);

                }
                j++;    
        }
        
        close(fd);

        return 0;
}
