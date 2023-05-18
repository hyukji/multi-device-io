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

void error_handling(char * message);

int main(int argc, char* argv[])
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
    pid_t pid;

    // create socket using TCP/ipv4
    int serv_sock;
    int clnt_sock;

    struct sockaddr_in serv_addr;

    serv_sock=socket(PF_INET, SOCK_STREAM, 0);  // TCP, ipv4
    if(serv_sock == -1)
        error_handling("socket error");

    memset(&serv_addr, 0, sizeof(serv_addr)); 
    serv_addr.sin_family=AF_INET;                // ipv4
    serv_addr.sin_addr.s_addr=htonl(INADDR_ANY); // ip address
    serv_addr.sin_port=htons(atoi(argv[1]));     // port from argu


   // bind socket to server addr
    if(bind(serv_sock, (struct sockaddr*) &serv_addr, sizeof(serv_addr))==-1)
        error_handling("bind error");


    // waiting for client req
    if(listen(serv_sock, 5)==-1)
        error_handling("listen error");

    while(1) {

        printf("waiting new client\n");
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

            fd = open(evdPath, O_RDWR);

            //   tcsetattr(STDIN_FILENO, TCSANOW, &newattr);
            if(fd < 0) {
                    perror("error");
                    return -1;
            }

            while(1){
                if((nbytes = read(clnt_sock, message, sizeof(message))) < 0){   
                    perror("read error\n");
                    close(clnt_sock);

                    printf("exit! pid : %d\n", getpid());
                    exit(0);
                }
                

                int j;
                if(strncmp(message, "q", 1) == 0){
                    j = 1;
                }else if(strncmp(message, "w", 1) == 0){
                    j = 17;
                } else if(strncmp(message, "a", 1) == 0){
                    j = 30;
                } else if(strncmp(message, "s", 1) == 0){
                    j = 31;
                } else if(strncmp(message, "d", 1) == 0){
                    j = 32;
                } else if(strncmp(message, "u", 1) == 0){
                    j = 22;
                } else if(strncmp(message, "i", 1) == 0){
                    j = 23;
                }

                iev[0].code = j;
                stp[0].code = j;
                write(fd, iev, sizeof(struct input_event));
                write(fd, syn, sizeof(struct input_event));
                write(fd, stp, sizeof(struct input_event));
                write(fd, syn, sizeof(struct input_event));
                usleep(100000);


                
                printf("pid : %d, data : %s\n", getpid(), message);
            }


            close(fd);
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
