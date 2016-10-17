#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <errno.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#define MAX_BUF 1024


int main(void) {
    int error_flag, 
        socket_fd, 
//        client_fd,
        address_len;

    struct sockaddr_in server_address;
    char buffer[MAX_BUF];

    socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_fd == -1) {
        perror("socket");
        exit(errno);
    }
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(7000);
    error_flag = inet_aton("127.0.0.1", 
            &server_address.sin_addr);
    if (error_flag == -1) {
        perror("inet_aton");
        exit(errno);
    }
    address_len = sizeof(struct sockaddr);
    error_flag = connect(socket_fd, 
            (struct sockaddr*)&server_address,
            address_len);
    if (error_flag == -1) {
        perror("connect");
        exit(errno);
    }
    strcpy(buffer, "Hello!\n\0");
    send(socket_fd, buffer, 8, 0);
    recv(socket_fd, buffer, MAX_BUF, 0);
    printf("%s", buffer);
    close(socket_fd);
    return 0;
}
