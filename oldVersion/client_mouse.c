#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <linux/input.h>

#include <arpa/inet.h>
#include <sys/socket.h>



void error_handling(char* message);

int main(int argc, char* argv[])
{
   int clnt_fd, evnt_fd;
   struct sockaddr_in serv_addr;
    struct input_event event;
    struct timeval tv;
    
    char buffer[sizeof(event)];

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

    // open keyboard event fd
    evnt_fd = open("/dev/input/event6", O_RDONLY);
    if(evnt_fd < 0) {
        perror("error in open event file");
        return 1;
    }
    
    while (1) {
        // read event of eventfd and send to server
        if(read(evnt_fd, &event, sizeof(event)) == -1) {
            perror("error in read event");
            close(evnt_fd);
            return 1;
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
