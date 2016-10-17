#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <errno.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <unistd.h>
#define MAX_QUEUE 5
#define MAX_BUF 1024

int make_socket(int port, char *address)
{
    int socket_fd, error_flag;
    struct sockaddr_in sock_address;

    socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_fd == -1) {
        perror("socket");
        exit(errno);
    }
    sock_address.sin_family = AF_INET;
    sock_address.sin_port = htons(port);
    sock_address.sin_addr.s_addr = inet_addr(address);
    //my_sockaddress.sin_addr.s_addr = INADDR_ANY;
    error_flag = bind(socket_fd, 
            (struct sockaddr *)&sock_address,
            sizeof(struct sockaddr));
    if (error_flag == -1) {
        perror("bind");
        exit(errno);
    }
    return socket_fd;
}


int main(void) {
    int error_flag, 
        socket_fd, 
        client_fd,
        address_len,
        sent_num,
        port;
    struct sockaddr_in client_address;
    char buffer[MAX_BUF], address[50];
    FILE *parameters;

    parameters = fopen("parameters.txt", "r");
    if (fscanf(parameters, "%d%s", &port, address) != 2) {
        perror("Error in parameters.txt");
        exit(errno);
    }

    socket_fd = make_socket(port, address);
    error_flag = listen(socket_fd, MAX_QUEUE);
    if (error_flag == -1) {
        perror("listen");
        exit(errno);
    }
    while (1) {
        client_fd = accept(socket_fd,
                (struct sockaddr*)&client_address,
                (socklen_t*)&address_len);
        if (client_fd == -1) {
            perror("accept");
            exit(errno);
        }
        printf("%s : %d connected\n", 
                inet_ntoa(client_address.sin_addr),
                ntohs(client_address.sin_port));
       /* printf("%s  connected\n", 
                (char *)inet_ntoa(client_address.sin_addr));
                ntohs(client_address.sin_port));*/

        sent_num = send(client_fd, buffer,
                recv(client_fd, buffer, MAX_BUF, 0), 0);
        if (sent_num == -1) {
            perror("send");
            exit(errno);
        }
        close(client_fd);
    }
    close (socket_fd);
    return 0;
}
        


