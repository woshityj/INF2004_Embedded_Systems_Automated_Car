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

// ---------------
// Linking with Ws2_32.lib for socket programming
// Use the -lws2_32 flag when compiling using GCC 
#pragma comment(lib, "ws2_32.lib")

int main()
{
    // ----------------------
    // Initialize Winsock
    //
    WSADATA wsaData;
    int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (iResult != NO_ERROR) {
        printf("WSAStartup function failed with error: %d\n", iResult);
        return 1;
    }

    // ----------------------
    // Create a SOCKET for connecting to server
    //
    SOCKET ConnectSocket;
    ConnectSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (ConnectSocket == INVALID_SOCKET) {
        printf("socket function failed with error: %ld\n", WSAGetLastError());
        WSACleanup();
        return 1;
    }
    // ----------------------
    // The sockaddr_in structure to specify address family,
    // IP address, and port of the server to be connected to.
    //
    SOCKADDR_IN clientService;
    clientService.sin_family = AF_INET;
    clientService.sin_addr.s_addr = inet_addr("192.168.31.232");
    clientService.sin_port = htons(PORT);

    // ----------------------
    // Connect to server
    // 
    iResult = connect(ConnectSocket, (SOCKADDR *) & clientService, sizeof (clientService));
    if (iResult == SOCKET_ERROR) {
        printf("connect function failed with error: %ld\n", WSAGetLastError());
        iResult = closesocket(ConnectSocket);
        if (iResult == SOCKET_ERROR)
            printf("closesocket function failed with error: %ld\n", WSAGetLastError());
        WSACleanup();
        return 1;
    }

    printf("Connected to server.\n");


    char* message = "Hello Server", reply[BUF_SIZE] = {0};

    // ---------------------------
    // Send message to server
    // 
    send(ConnectSocket, message, BUF_SIZE, 0);
    printf("Message sent\n");

    // ---------------------------
    // Receive reply from server
    // 
    int bytesReceived = recv(ConnectSocket, reply, BUF_SIZE, 0);
    printf("Message from server: %s\n", reply);

    // ----------------------
    // Closing socket
    // 
    iResult = closesocket(ConnectSocket);
    if (iResult == SOCKET_ERROR) {
        printf("closesocket function failed with error: %ld\n", WSAGetLastError());
        WSACleanup();
        return 1;
    }
    printf("Socket closed\n");

    WSACleanup();
    return 0;
}