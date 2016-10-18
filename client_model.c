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
        port,
        address_len;
    char ip[50];
    struct sockaddr_in server_address;
    char buffer[MAX_BUF];
    FILE *parameters;

    parameters = fopen("client_parameters.txt", "r");

    error_flag = fscanf(parameters, "%d%s", &port, ip);
    if (error_flag != 2) {
        perror("Error in parameters");
        exit(errno);
    }
    fclose(parameters);
    socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_fd == -1) {
        perror("socket");
        exit(errno);
    }
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(port);
    error_flag = inet_aton(ip, 
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
    //strcpy(buffer, "Hello!\n\0");
    //send(socket_fd, buffer, 8, 0);
    //recv(socket_fd, buffer, MAX_BUF, 0);
    //printf("%s", buffer);
    while(scanf("%s", buffer)) {
        error_flag = write(socket_fd, buffer, strlen(buffer) + 1);
        if (error_flag == (-1)) {
            perror("write");
            exit(errno);
        } else if (error_flag == 0) {
            printf("Server has gone");
            break;
        }
    }
    shutdown(socket_fd, 2);
    close(socket_fd);
    return 0;
}
