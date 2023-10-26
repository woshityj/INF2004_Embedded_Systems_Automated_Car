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
    char* hello = "exit";
    if ((client_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("\nSocket creation error\n");
        return -1;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    if (inet_pton(AF_INET, "192.168.0.209", &serv_addr.sin_addr) <= 0)
    {
        printf("\nInvalid address / Address not supported\n");
        return -1;
    }

    if ((status = connect(client_fd, (struct sockaddr*)&serv_addr, sizeof(serv_addr))) < 0)
    {
        printf("\nConnection Failed\n");
        return -1;
    }

    int total_size = 2048;
    char buffer[2048] = { 0 };

    while (total_size > 0)
    {
        valread = read(client_fd, buffer, 2048 - 1);
        printf("%s\n", buffer);
        printf("read %d bytes from server\n", valread);
        total_size -= valread;
    }

    sleep(3);
    send(client_fd, hello, 2048, 0);
    printf("Hello message sent\n");

    close(client_fd);
    return 0;
}