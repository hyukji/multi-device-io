#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <termios.h>
#include <arpa/inet.h>
#include <sys/socket.h>
void error_handling(char* message);

int main(int argc, char* argv[])
{
   int clnt_sock;
   struct sockaddr_in serv_addr;
   char message[1024] = {0x00, };
       int nbytes;
       
       
   // setting for direct input
   char c;
   char buf[2];
   struct termios oldattr, newattr;
   
   tcgetattr(STDIN_FILENO, &oldattr);
   newattr = oldattr;
   newattr.c_lflag &= ~(ICANON | ECHO);
   newattr.c_cc[VMIN] = 1;
   newattr.c_cc[VTIME] = 0;
   tcsetattr(STDIN_FILENO, TCSANOW, &newattr);

   //TCP연결지향형이고 ipv4 도메인을 위한 소켓을 생성
   clnt_sock = socket(PF_INET, SOCK_STREAM, 0);
   if(clnt_sock == -1)
      error_handling("socket error");

   memset(&serv_addr, 0, sizeof(serv_addr));
   serv_addr.sin_family = AF_INET;            
   serv_addr.sin_addr.s_addr = inet_addr(argv[1]);  
   serv_addr.sin_port = htons(atoi(argv[2]));

   //클라이언트 소켓부분에 서버를 연결!
   if(connect(clnt_sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) == -1)
      error_handling("connect error");

   printf("clent connect success!\n");   
   
   //연결이 성공적으로 되었으면 데이터 받기
   while(1) {
      c = getchar();
      buf[0] = c;

      printf("input data : %s\n", buf);
      write(clnt_sock, buf, 2); 
      
      if(!strcmp(buf, "q")) {
         break;
      }
   }
    
   //통신 후 소켓 클로우즈
   close(clnt_sock);
   return 0;
}
void error_handling(char* message)
{
   fputs(message, stderr);
   fputc('\n', stderr);
   exit(1);
}