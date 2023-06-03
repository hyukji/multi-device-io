#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <linux/input.h>

#include <arpa/inet.h>
#include <sys/socket.h>
#include "winKeyboardInputEvent.h"


void error_handling(char* message);

int main(int argc, char* argv[])
{

	int clnt_fd, evnt_fd;
	struct WinKBD_input_event window_event;
	struct sockaddr_in serv_addr;
	struct input_event event;
	struct timeval tv;

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

/*    write(clnt_fd, OS, sizeof(OS));*/

	char OS[5] = "unix";
	
    write(clnt_fd, OS, sizeof(OS));
	
    /*if(send(clnt_fd, OS, 5, 0) < 0)
	{
		puts("Send failed");
		return 1;
	}
    */
    
    int device;
    printf("Enter a input device(kbd : 0, mouse : 1) : ");
    scanf("%d", &device);
    write(clnt_fd, &device, sizeof(device));
    
    printf("%d\n", device);
    switch(device) {
	case 0: //kbd
    printf("0\n");
	    evnt_fd = open("/dev/input/event1", O_RDONLY);
	    break;
	case 1: //mouse
    printf("1\n");
	    evnt_fd = open("/dev/input/event0", O_RDONLY);
	    break;
    }

    		// open keyboard event fd
	if(evnt_fd < 0) {
		perror("error in open event file");
		return 1;
	}
	
	printf("evnt fd %d\n", evnt_fd);
	while(1) {
		if(read(evnt_fd, &event, sizeof(event)) == -1) {
		    perror("error in read event");
		    close(evnt_fd);
		    break;
		} else {
		    tv = event.time;
		    printf("time %ld %ld, type %d, code %d, value %d\n", tv.tv_sec, tv.tv_usec,  event.type, event.code, event.value);
		    
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
