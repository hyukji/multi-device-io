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

void error_handling(char *message);

// 스레드 실행 함수
void *client_handler(void *arg)
{
    const char* evdPath = "/dev/input/event3";
    struct input_event iev[1], syn[1], stp[1];

    iev[0].value = 1;
    iev[0].type = EV_KEY;

    syn[0].type = EV_SYN;
    syn[0].code = 0;
    syn[0].value = 0;

    stp[0].type = EV_KEY;
    stp[0].value = 0;

    char message[2] = {0, };
    int nbytes;
    int fd;

    // 현재 스레드의 클라이언트 소켓 가져오기
    int clnt_sock = *((int *)arg);
    free(arg);  // 동적 할당된 메모리 해제

    printf("connected! tid : %ld\n", pthread_self());

    const char* evdPath = "/dev/input/event3";
    fd = open(evdPath, O_RDWR);

    if (fd < 0) {
        perror("error");
        pthread_exit(NULL);
    }

    while (1) {
        if ((nbytes = read(clnt_sock, message, sizeof(message))) < 0) {
            perror("read error\n");
            close(clnt_sock);
            printf("exit! tid : %ld\n", pthread_self());
            pthread_exit(NULL);
        }

        int j;
        if (strncmp(message, "q", 1) == 0) {
            j = 1;
        } else if (strncmp(message, "w", 1) == 0) {
            j = 17;
        } else if (strncmp(message, "a", 1) == 0) {
            j = 30;
        } else if (strncmp(message, "s", 1) == 0) {
            j = 31;
        } else if (strncmp(message, "d", 1) == 0) {
            j = 32;
        } else if (strncmp(message, "u", 1) == 0) {
            j = 22;
        } else if (strncmp(message, "i", 1) == 0) {
            j = 23;
        }

        iev[0].code = j;
        stp[0].code = j;
        write(fd, iev, sizeof(struct input_event));
        write(fd, syn, sizeof(struct input_event));
        write(fd, stp, sizeof(struct input_event));
        write(fd, syn, sizeof(struct input_event));
        usleep(100000);

        printf("tid : %ld, data : %s\n", pthread_self(), message);
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
    if (serv_sock == -1)
        error_handling("socket error");

    memset(&serv_addr, 0, sizeof(serv_addr)); 
    serv_addr.sin_family = AF_INET;                // ipv4
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY); // ip address
    serv_addr.sin_port = htons(atoi(argv[1]));     // port from argu

   // bind socket to server addr
    if (bind(serv_sock, (struct sockaddr*) &serv_addr, sizeof(serv_addr)) == -1)
        error_handling("bind error");

    // waiting for client req
    if (listen(serv_sock, 5) == -1)
        error_handling("listen error");

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

void error_handling(char *message)
{
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(1);
}
