#ifndef UNICODE
#define UNICODE
#endif

#define WIN32_LEAN_AND_MEAN
#define BUF_SIZE 2048
#define TEST_ITERATIONS 10
#define PORT 4242

#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdio.h>
#include <string.h>

// Need to link with Ws2_32.lib
#pragma comment(lib, "ws2_32.lib")

int main()
{
    //----------------------
    // Initialize Winsock
    WSADATA wsaData;
    int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (iResult != NO_ERROR) {
        wprintf(L"WSAStartup function failed with error: %d\n", iResult);
        return 1;
    }
    //----------------------
    // Create a SOCKET for connecting to server
    SOCKET ConnectSocket;
    ConnectSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (ConnectSocket == INVALID_SOCKET) {
        wprintf(L"socket function failed with error: %ld\n", WSAGetLastError());
        WSACleanup();
        return 1;
    }
    //----------------------
    // The sockaddr_in structure specifies the address family,
    // IP address, and port of the server to be connected to.
    SOCKADDR_IN clientService;
    clientService.sin_family = AF_INET;
    clientService.sin_addr.s_addr = inet_addr("192.168.79.12");
    clientService.sin_port = htons(PORT);

    //----------------------
    // Connect to server.
    iResult = connect(ConnectSocket, (SOCKADDR *) & clientService, sizeof (clientService));
    if (iResult == SOCKET_ERROR) {
        wprintf(L"connect function failed with error: %ld\n", WSAGetLastError());
        iResult = closesocket(ConnectSocket);
        if (iResult == SOCKET_ERROR)
            wprintf(L"closesocket function failed with error: %ld\n", WSAGetLastError());
        WSACleanup();
        return 1;
    }

    wprintf(L"Connected to server.\n");

    char message[BUF_SIZE], reply[BUF_SIZE];
    for (int i = 0; i < TEST_ITERATIONS; i++){
        printf("Test iteration number %d\n", i+1);

        // ---------------------------
        // Receive reply from server
        int bytesReceived = recv(ConnectSocket, reply, BUF_SIZE, 0);
        printf("read %d bytes from server\n", bytesReceived);
        
        // ----------------------------
        // Copy reply to message buffer 
        memcpy(message, reply, BUF_SIZE);

        // ---------------------------
        // Send message to server
        send(ConnectSocket, message, BUF_SIZE, 0);
        printf("Message sent of %d bytes\n", (sizeof(message)));
    }
    
    printf("End of test\n");

    iResult = closesocket(ConnectSocket);
    if (iResult == SOCKET_ERROR) {
        wprintf(L"closesocket function failed with error: %ld\n", WSAGetLastError());
        WSACleanup();
        return 1;
    }
    printf("Socket closed\n");

    WSACleanup();
    return 0;
}