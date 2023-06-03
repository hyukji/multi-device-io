#include <stdio.h>
#include <stdlib.h>
#include <winsock2.h>
#include "winKeyboardInputEvent.h"

// gcc -o client.exe client.c -lws2_32 -Wall

int main(int argc, char* argv[]) {
    WSADATA wsaData;
    SOCKET clientSocket;
    struct sockaddr_in serverAddress;

	struct WinKBD_input_event window_event;

    // initialize Winsock.
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        printf("Failed to initialize Winsock.\n");
        return 1;
    }

    // create socket
    clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket == INVALID_SOCKET) {
        printf("Failed to create socket.\n");
        WSACleanup();
        return 1;
    }

    // set socket addr
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = inet_addr(argv[1]);
    serverAddress.sin_port = htons(9090);

    // connect to server
    if (connect(clientSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) == SOCKET_ERROR) {
        printf("Failed to connect to the server.\n");
        closesocket(clientSocket);
        WSACleanup();
        return 1;
    }

    printf("Connected to the server.\n");

    // send to server about OS
    char OS[5] = "wind";
    if(send(clientSocket, OS, 5, 0) < 0)
	{
		puts("Send failed");
		return 1;
	}

    int msg;
    while(1) {
        // get kbd input by getch()
        msg = getch(); // return value is ascii code

        int corr = 1;
        switch(msg) {
            case 113:  // q
                window_event.value = 16;
                break;
            case 32:  // space bar
                window_event.value = 16;
                break;
            case 13: // enter
                window_event.value = 28;
                break;
            default:
                corr = 0;
        }
        if(!corr) { continue; }
        
        // send data
        window_event.state = 1;
        send(clientSocket, &window_event, sizeof(window_event), 0);

        window_event.state = 0;
        send(clientSocket, &window_event, sizeof(window_event), 0);
    }

    // 소켓 및 Winsock 종료
    closesocket(clientSocket);
    WSACleanup();

    return 0;
}
