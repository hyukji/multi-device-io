#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <linux/input.h>
#include <signal.h>
#include <arpa/inet.h>
#include <sys/socket.h>


int* ptr;

void error_handling(char* message)
{
   fputs(message, stderr);
   fputc('\n', stderr);
   exit(1);
}

void sig_handle(){
    *ptr = -1;
} 


    
int main(int argc, char* argv[])
{
    int clnt_fd, evnt_fd, loop, nbytes;
    struct sockaddr_in serv_addr;
    struct input_event event;
    struct timeval tv;
    
    char buffer[sizeof(event)];
    evnt_fd = open(argv[2], O_RDONLY);

    char *inPath = (char *)malloc(sizeof(getpid())*2);
    sprintf(inPath,"%ul.out", getpid());

    FILE * fp = fopen(inPath, "w+");

    if(fp == NULL) {
        perror("error in open file");
        return 1;
    }

    if(evnt_fd < 0) {
        perror("error in open event file");
        return 1;
    }

    signal(SIGINT, sig_handle);
    loop = 1;
    ptr = &loop;

    // socket with TCP/IP, IPv4
    clnt_fd = socket(PF_INET, SOCK_STREAM, 0);
    if(clnt_fd == -1) error_handling("socket error");

    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;            
    serv_addr.sin_addr.s_addr = inet_addr(argv[1]);  
    serv_addr.sin_port = htons(9090);

    // connect clnt socket to server socket
    if(connect(clnt_fd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) == -1) error_handling("connect error");
      
    printf("clent connect success!\n");   

    while (loop > 0) {
	    
	nbytes = read(evnt_fd, &event, sizeof(event));
	// Handle READ ERROR
        if(nbytes == -1) {
            perror("error in read event");
            close(evnt_fd);
   	    close(clnt_fd);
            return 1;
        } 
	
	// BREAK when reach to the end of the file
	if(nbytes == 0){
	    printf("FILE READ END");
	    break;
	}

	tv = event.time;
        printf("time %ld %ld, type %d, code %d, value %d\n", tv.tv_sec, tv.tv_usec, event.type, event.code, event.value);
        //fprintf(fp, "time %ld %ld, type %u, code %u, value %u\n", tv.tv_sec, tv.tv_usec, event.type, event.code, event.value);
        fprintf(fp, "type %u, code %u, value %u\n", event.type, event.code, event.value);
        write(clnt_fd, &event, sizeof(event));
    }

    fclose(fp);
   close(clnt_fd);
   close(evnt_fd);

   return 0;
}


