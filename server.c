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
#include <pthread.h>  // pthread 라이브러리 추가
#include "winKeyboardInputEvent.h"

#define evdPath_kbd "/dev/input/event0"
#define evdPath_mouse "/dev/input/event1"

void error_handling(char *message);

struct thread_struct {
	int* socket;
	int evnt;
};


// handler for window clinet
void *client_handler_window(void *arg)
{    
    int fd, nbytes;
    struct WinKBD_input_event window_event;
    struct timeval tv;
    struct input_event event[3] = {
    	{tv, 4, 4, 0},
    	{tv, 1, 0, 0},
    	{tv, 0, 0, 0}
    };

    
    // 현재 스레드의 클라이언트 소켓 가져오기
    struct thread_struct evnt_struct = *((struct thread_struct *)arg);
    
    int clnt_sock = *(evnt_struct.socket);
    free(arg);  // 동적 할당된 메모리 해제
    
    fd = open(evdPath_kbd, O_RDWR);
    printf("fd %d\n", fd);
    
    printf("window client connected! tid : %ld\n", pthread_self());
    printf("fd %d\n", fd);

    // open event fd
    if (fd < 0) {
        perror("error");
        pthread_exit(NULL);
    }

    while(1){
        // get buffer about event from client
        nbytes = read(clnt_sock, &window_event, sizeof(window_event));
        if(nbytes < 0){
            perror("read error\n");
            close(clnt_sock);
            printf("exit! tid : %ld\n", pthread_self());
            pthread_exit(NULL);
        }
        
	gettimeofday(&tv, NULL);
	event[0].time = tv;
	event[0].value = window_event.value;
	
	event[1].time = tv;
	event[1].code = window_event.value;
	event[1].value = window_event.state;
	
	event[2].time = tv;
	
	// compare before event for 2
			
	// write event to event fd
	for(int i = 0; i < 3; i++){
		write(fd, &event[i], sizeof(event[i]));
		
		printf("pid %d, tid %ld, time %ld %ld, type %d, code %d, value %d\n", getpid() ,pthread_self(), tv.tv_sec, tv.tv_usec,  event[i].type, event[i].code, event[i].value);
	}
    }

    close(fd);
    close(clnt_sock);

    pthread_exit(NULL);
}



// 스레드 실행 함수
void *client_handler_unix(void *arg)
{    
    int fd, nbytes;
    struct input_event event;
    struct timeval tv;
    
    
    struct thread_struct evnt_struct = *((struct thread_struct *)arg);
    
    int clnt_sock = *(evnt_struct.socket);
    free(arg);  // 동적 할당된 메모리 해제
    
    switch(evnt_struct.event) {
    	case 0:
    		fd = open(evdPath_kbd, O_RDWR);
    		break;	
    	case 1:
		fd = open(evdPath_mouse, O_RDWR);
    		break;
    }
    printf("fd %d\n", fd);
    

    printf("unix client connected! tid : %ld\n", pthread_self());
    printf("fd %d\n", fd);

    // open event fd
    if (fd < 0) {
        perror("error");
        pthread_exit(NULL);
    }

    while(1){
        // get buffer about event from client
        nbytes = read(clnt_sock, &event, sizeof(event));
        if(nbytes < 0){
            perror("read error\n");
            close(clnt_sock);
            printf("exit! tid : %ld\n", pthread_self());
            pthread_exit(NULL);
        }
        
        
        // write event to event fd
        write(fd, &event, sizeof(event));

        tv = event.time;
        printf("pid %d, tid %ld, time %ld %ld, type %d, code %d, value %d\n", getpid() ,pthread_self(), tv.tv_sec, tv.tv_usec,  event.type, event.code, event.value);
    }

    close(fd);
    close(clnt_sock);

    pthread_exit(NULL);
}

int main(int argc, char* argv[])
{          
    // create socket using TCP/ipv4
    int serv_sock;
    int clnt_sock;
    
    struct sockaddr_in serv_addr;

    serv_sock = socket(PF_INET , SOCK_STREAM, 0);  // TCP, ipv4
    if (serv_sock == -1)
        error_handling("socket error");

    memset(&serv_addr, 0, sizeof(serv_addr)); 
    serv_addr.sin_family = AF_INET;                // ipv4
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY); // ip address
    serv_addr.sin_port = htons(9090);     // port from argu

    int reuse = 1;
    setsockopt(serv_sock, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof reuse);
    
   // bind socket to server addr
    if (bind(serv_sock, (struct sockaddr*) &serv_addr, sizeof(serv_addr)) == -1)
        error_handling("bind error");

    // waiting for client req
    if (listen(serv_sock, 5) == -1)
        error_handling("listen error");// check port number

    char ip_addr[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &(serv_addr.sin_addr), ip_addr, INET_ADDRSTRLEN);
    printf("Server address(IPv4) is %s\n", ip_addr);
    printf("waiting new client...\n");

    while (1) {
        struct sockaddr_in clnt_addr; // for accept
        socklen_t clnt_addr_size;

        // accept connection with client 
        clnt_addr_size = sizeof(clnt_addr);
        clnt_sock = accept(serv_sock, (struct sockaddr*)&clnt_addr, &clnt_addr_size);
        if (clnt_sock == -1)
            error_handling("accept error");
            
            
	char OS[5];
	recv(clnt_sock, OS, 5, 0);

        // 클라이언트 소켓을 스레드에 전달하기 위해 동적 할당
        int* new_sock = (int *)malloc(sizeof(int));
        *new_sock = clnt_sock;

        // 스레드 생성 및 실행
        pthread_t tid;
     
        if(!strcmp(OS, "wind")) {
        	struct thread_struct th_arg = { new_sock, 0 }; // 0: kbd, 1: mouse
		if (pthread_create(&tid, NULL, client_handler_window, (void*)&th_arg) != 0) {
			perror("pthread_create() error\n");
			exit(1);
		}
        }
        else if(!strcmp(OS, "unix")) {
                    
		int evnt;
		if(read(clnt_sock, &evnt, sizeof(evnt)) == -1) {
			print("select keyboad : 0, mouse :1\n");
		}

        	struct thread_struct th_arg = { new_sock, evnt }; // 0: kbd, 1: mouse
	    if (pthread_create(&tid, NULL, client_handler_unix, (void *)&th_arg) != 0) {
		    perror("pthread_create() error\n");
		    exit(1);
	    }
	}
        else {
		printf("Wrong OS %s\n", OS);
		   break;
        }
    }
    
    close(serv_sock);
    return 0;
}

void error_handling(char *message)
{
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(1);
}
