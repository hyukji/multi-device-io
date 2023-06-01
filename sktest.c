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

int *ptr;

void error_handling(char *message)
{
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(1);
}

void sig_handler() *ptr = 1;
// 스레드 실행 함수
void *client_handler(void *arg)
{
    int fd;
    int nbytes;
    int loop = 1;
    struct input_event event;
    struct timeval tv;

    ptr = &loop;
    signal(SIGINT, sig_handler);
    // 현재 스레드의 클라이언트 소켓 가져오기
    int clnt_sock = *((int *)arg);
    free(arg);  // 동적 할당된 메모리 해제
    
    printf("connected! tid : %ld\n", pthread_self());
    
    const char *evdPath = "/dev/input/event4"; //Need to change event file
    fd = open(evdPath, O_RDWR);
    
    if (fd < 0) {
        perror("error");
        pthread_exit(NULL);
    }
    
    while (loop > 0) {
        // get buffer about event from client
        nbytes = read(clnt_sock, &event, sizeof(event));
        if (nbytes < 0) {
            perror("read error\n");
            close(clnt_sock);
            printf("exit! tid : %ld\n", pthread_self());
            pthread_exit(NULL);
        }
        
        write(fd, &event, sizeof(event));
	
        tv = event.time;
        printf("tid %lu, time %ld %ld, type %d, code %d, value %d\n", pthread_self(), tv.tv_sec, tv.tv_usec, event.type, event.code, event.value);

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

    serv_sock = socket(PF_INET, SOCK_STREAM, 0);  // TCP, ipv4
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
    if(listen(serv_sock, 5)==-1) error_handling("listen error");

    char ip_addr[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &(serv_addr.sin_addr), ip_addr, INET_ADDRSTRLEN);
    printf("Server address(IPv4) is %s\n", ip_addr);

    while (1) {
        printf("waiting new client\n");
        struct sockaddr_in clnt_addr; // for accept
        socklen_t clnt_addr_size;

        // accept connection with client 
        clnt_addr_size = sizeof(clnt_addr);
        clnt_sock = accept(serv_sock, (struct sockaddr*)&clnt_addr, &clnt_addr_size);
        if (clnt_sock == -1)
            error_handling("accept error");

        // 클라이언트 소켓을 스레드에 전달하기 위해 동적 할당
        int *new_sock = (int *)malloc(sizeof(int));
        *new_sock = clnt_sock;

        // 스레드 생성 및 실행
        pthread_t tid;
        if (pthread_create(&tid, NULL, client_handler, (void *)new_sock) != 0) {
            perror("pthread_create() error\n");
            exit(1);
        }
    }
    
    close(serv_sock);
    return 0;
}



