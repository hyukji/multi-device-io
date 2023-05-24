#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <unistd.h>
#include <linux/input.h>

#include <arpa/inet.h>
#include <sys/socket.h>

#include <sys/types.h>
#include <sys/stat.h>

void error_handling(char * message);

int main(int argc, char* argv[])
{          
   int clnt_fd, evnt_fd;
    int nbytes;
    pid_t pid;

    // create socket using TCP/ipv4
    int serv_sock;
    int clnt_sock;
    struct input_event event;
    struct timeval tv;

    struct sockaddr_in serv_addr;

    serv_sock=socket(PF_INET, SOCK_STREAM, 0);  // TCP, ipv4
    if(serv_sock == -1)
        error_handling("socket error");

    memset(&serv_addr, 0, sizeof(serv_addr)); 
    serv_addr.sin_family=AF_INET;                // ipv4
    serv_addr.sin_addr.s_addr=htonl(INADDR_ANY); // ip address
    serv_addr.sin_port=htons(9090);     // port from argu

    // check port number
    int reuse = 1;
    setsockopt(serv_sock, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof reuse);

    // bind socket to server addr
    if(bind(serv_sock, (struct sockaddr*) &serv_addr, sizeof(serv_addr))==-1)
        error_handling("bind error");

    // waiting for client req
    if(listen(serv_sock, 5)==-1)
        error_handling("listen error");

    char ip_addr[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &(serv_addr.sin_addr), ip_addr, INET_ADDRSTRLEN);
    printf("Server address(IPv4) is %s\n", ip_addr);
    printf("waiting new client...\n");
    
    while(1) {
        struct sockaddr_in clnt_addr; // for accept
        socklen_t clnt_addr_size;

        // accept connection with client 
        clnt_addr_size = sizeof(clnt_addr);
        clnt_sock=accept(serv_sock, (struct sockaddr*)&clnt_addr, &clnt_addr_size);
        if(clnt_sock==-1)
            error_handling("accept error");


        if((pid = fork()) == -1){
            close(clnt_sock);
            perror("fork() error\n");
        } 
        else if (pid == 0){    //child process
            close(serv_sock);
            
            printf("connected! pid : %d\n", getpid());
            
            // open event fd
            evnt_fd = open("/dev/input/event2", O_RDONLY);
	    if(evnt_fd < 0) {
		perror("error in open event file");
		return 1;
	    }

            while(1){
            	// get buffer about event from client
            	nbytes = read(clnt_sock, &event, sizeof(event));
                if(nbytes < 0){   
                    perror("read error\n");
                    close(clnt_sock);

                    printf("exit! pid : %d\n", getpid());
                    break;
                }
                
                // write event to event fd
            	write(evnt_fd, &event, sizeof(event));
            
		tv = event.time;
		printf("pid %d, time %ld %ld, type %d, code %d, value %d\n", getpid(), tv.tv_sec, tv.tv_usec,  event.type, event.code, event.value);
            }
            
	    close(evnt_fd);
        }
    }
    
    close(serv_sock);
    return 0;
}

void error_handling(char *message)
{
    fputs(message,stderr);
    fputc('\n', stderr);
    exit(1);
}
