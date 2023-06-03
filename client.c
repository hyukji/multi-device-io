#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <linux/input.h>

#include <arpa/inet.h>
#include <sys/socket.h>
#include "winKeyboardInputEvent.h"

#define evdPath_kbd "/dev/input/event1" //keyboard
#define evdPath_mouse "/dev/input/event0" //mouse

void error_handling(char* message);

int main(int argc, char* argv[])
{
    int clnt_fd, evnt_fd, device;
    struct WinKBD_input_event window_event;
    struct sockaddr_in serv_addr;
    struct input_event event;
    struct timeval tv;
    
    char OS[5] = "unix";

    // socket with TCP/IP, IPv4
    clnt_fd = socket(PF_INET, SOCK_STREAM, 0);
    if(clnt_fd == -1)
        error_handling("socket error");

    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr(argv[1]);
    serv_addr.sin_port = htons(9090);

    // connect clnt socket to server socket
    if(connect(clnt_fd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) == -1)
        error_handling("connect erro3r");

    printf("clent connect success!\n");

    // send OS data to server
    write(clnt_fd, OS, sizeof(OS));
    
    printf("Enter a input device(kbd : 0, mouse : 1) : ");
    scanf("%d", &device);
    write(clnt_fd, &device, sizeof(device));
    
    // open event file descriptor
    switch(device) {
        case 0: //kbd
            evnt_fd = open("/dev/input/event1", O_RDONLY);
            break;
        case 1: //mouse
            evnt_fd = open("/dev/input/event0", O_RDONLY);
            break;
    }

    // open keyboard event fd
    if(evnt_fd < 0) {
        perror("error in open event file");
        return 1;
    }
    
    while(1) {
        // read data from event file and send to server
        if(read(evnt_fd, &event, sizeof(event)) == -1) {
            perror("error in read event");
            close(evnt_fd);
            break;
        } else {
            tv = event.time;
            write(clnt_fd, &event, sizeof(event));
        }
    }
    
   close(clnt_fd);
   close(evnt_fd);

   return 0;
}


void error_handling(char* message)
{
   fputs(message, stderr);
   fputc('\n', stderr);
   exit(1);
}
