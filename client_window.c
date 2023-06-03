#include <stdio.h>
#include <stdlib.h>
#include <winsock2.h>
#include "winKeyboardInputEvent.h"

// gcc -o client.exe client.c -lws2_32 -Wall
#define BUFFER_SIZE 1024

int main(int argc, char* argv[]) {
    WSADATA wsaData;
    SOCKET clientSocket;
    struct sockaddr_in serverAddress;

	struct WinKBD_input_event window_event;

    // WSAStartup() 함수를 호출하여 Winsock 라이브러리 초기화
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        printf("Failed to initialize Winsock.\n");
        return 1;
    }

    // 클라이언트 소켓 생성
    clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket == INVALID_SOCKET) {
        printf("Failed to create socket.\n");
        WSACleanup();
        return 1;
    }

    // 서버 주소 설정
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = inet_addr(argv[1]);
    serverAddress.sin_port = htons(9090);

    // 서버에 연결
    if (connect(clientSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) == SOCKET_ERROR) {
        printf("Failed to connect to the server.\n");
        closesocket(clientSocket);
        WSACleanup();
        return 1;
    }

    printf("Connected to the server.\n");

    // 서버로 데이터 전송
    char OS[5] = "wind";
    if(send(clientSocket, OS, 4, 0) < 0)
	{
		puts("Send failed");
		return 1;
	}

    while(1) {
        window_event.state = 1;
        window_event.value = 17;
        send(clientSocket, &window_event, sizeof(window_event), 0);
    
        window_event.state = 0;
        window_event.value = 17;
        send(clientSocket, &window_event, sizeof(window_event), 0);
printf("send");
        sleep(14);
    }

    // 소켓 및 Winsock 종료
    closesocket(clientSocket);
    WSACleanup();

    return 0;
}
