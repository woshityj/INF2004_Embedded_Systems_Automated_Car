#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#define PORT 4242
#define TEST_ITERATIONS 10


int main(int argc, char const* argv[])
{
    int status, valread, client_fd;
    struct sockaddr_in serv_addr;
    char* hello = "Hello Server";

    // ----------------------
    // Create a SOCKET for connecting to server
    //
    if ((client_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("\nSocket creation error\n");
        return -1;
    }


    // ----------------------
    // The sockaddr_in structure to specify address family,
    // IP address, and port of the server to be connected to.
    //
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    if (inet_pton(AF_INET, "192.168.0.209", &serv_addr.sin_addr) <= 0)
    {
        printf("\nInvalid address / Address not supported\n");
        return -1;
    }

    // ----------------------
    // Connect to server
    // 
    if ((status = connect(client_fd, (struct sockaddr*)&serv_addr, sizeof(serv_addr))) < 0)
    {
        printf("\nConnection Failed\n");
        return -1;
    }

    int total_size = 2048;
    char buffer[2048] = { 0 };

    // ---------------------------
    // Send message to server
    // 
    send(client_fd, hello, strlen(hello), 0);
    printf("%s message sent\n", hello);
    sleep(3);

    // ---------------------------
    // Receive reply from server
    // 
    valread = read(client_fd, buffer, 2048 - 1);
    printf("%s\n", buffer);
    printf("read %d bytes from server\n", valread);

    // ----------------------
    // Closing socket
    // 
    close(client_fd);
    return 0;
}