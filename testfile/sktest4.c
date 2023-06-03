#include <stdio.h>
#include <stdlib.h> 
#include <string.h> 
#include <unistd.h> 
#include <arpa/inet.h>  
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <linux/input.h>
#include <sys/time.h>
#include <pthread.h>
#include <signal.h>


struct args {
    int sock;
    struct sockaddr *serv_addr;
    unsigned int addr_size;
};

void error_handling(char *message)
{
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(1);
}

void *client_handler(void *arg)
{
    int fd, fd2;
    int nbytes, wbytes;
    struct input_event event;
    struct timeval tv;
    struct args clnt_sock = *((struct args *)arg);
    free(arg);      
    struct input_event *buffer = (struct input_event *) malloc(sizeof(event));
    printf("connected! tid : %ld\n", pthread_self());
    const char msg[20] = "DONE";
    const char *evdPath = "/dev/input/event3"; //Need to change event file

    char *rstPath = (char *)malloc(sizeof(pthread_self())*2);
    sprintf(rstPath,"%ld.out", pthread_self());

    fd = open(evdPath, O_RDWR);

    if (fd < 0) {
        perror("error");
        pthread_exit(NULL);
    }

    if (fd2 < 0) {
        perror("error");
        pthread_exit(NULL);
    }
    
    FILE * fp = fopen(rstPath, "w+");

    if(fp == NULL) {
        perror("error");
        pthread_exit(NULL);
    }


    while (1) {

        //nbytes = recv(clnt_sock, buffer, sizeof(event),0);
	recvfrom(clnt_sock.sock, buffer, sizeof(event), 0, clnt_sock.serv_addr, &clnt_sock.addr_size);
        if (nbytes < 0) {
            perror("read error\n");
            printf("exit! tid : %ld\n", pthread_self());
	    break;
        }else if(nbytes > 0){
	   //wbytes = send(clnt_sock, msg, sizeof(msg), 0);
	   if(wbytes < 0){
                perror("response error\n");
                printf("exit! tid : %ld\n", pthread_self());
		break;
	   }
	}

        //if(recv(clnt_sock, &event, sizeof(event), MSG_PEEK | MSG_DONTWAIT) == 0) break;

        write(fd, buffer, sizeof(event));
	
        tv = event.time;
        printf("tid %lu, time %ld %ld, type %ul, code %ul, value %ul\n", pthread_self(), tv.tv_sec, tv.tv_usec, event.type, event.code, event.value);
        fprintf(fp,"type %u, code %u, value %u\n", event.type, event.code, event.value);

    }

    close(fd);
    fclose(fp);
    //close(clnt_sock);
    free(rstPath);
    pthread_exit(NULL);
}


int main(int argc, char* argv[])
{          

    int serv_sock;
    int clnt_sock;
    struct sockaddr_in serv_addr;

    serv_sock = socket(PF_INET, SOCK_DGRAM, 0);  // TCP, ipv4
    if (serv_sock == -1) error_handling("socket error");

    memset(&serv_addr, 0, sizeof(serv_addr)); 
    serv_addr.sin_family = AF_INET;                // ipv4
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY); // ip address
    serv_addr.sin_port = htons(atoi(argv[1]));     // port from argu

    // bind socket to server addr
    if(bind(serv_sock, (struct sockaddr*) &serv_addr, sizeof(serv_addr))==-1)
        error_handling("bind error");

    // setsockopt for port reuse
    int reuse = 1;
    if (setsockopt(serv_sock, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) == -1) error_handling("setsockopt error");

    // waiting for client req
    //if(listen(serv_sock, 5)==-1) error_handling("listen error");

    char ip_addr[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &(serv_addr.sin_addr), ip_addr, INET_ADDRSTRLEN);
    printf("Server address(IPv4) is %s\n", ip_addr);

    while (1) {
        printf("waiting new client\n");
        struct sockaddr_in clnt_addr; // for accept
        socklen_t clnt_addr_size;

        clnt_addr_size = sizeof(clnt_addr);
        //clnt_sock = accept(serv_sock, (struct sockaddr*)&clnt_addr, &clnt_addr_size);
        //if (clnt_sock == -1) error_handling("accept error");
        struct args *new_sock = (struct args *)malloc(sizeof(struct args));
	new_sock->sock = serv_sock;
	new_sock->serv_addr = (struct sockaddr*) &serv_addr;
	new_sock->addr_size = sizeof(serv_addr);

        pthread_t tid;
        if (pthread_create(&tid, NULL, client_handler, (void *)new_sock) != 0) {
            perror("pthread_create() error\n");
            exit(1);
        }
    }
    
    close(serv_sock);
    return 0;
}



