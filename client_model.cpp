#include "client.h"

Client:: Client() {
    FILE *parameters;
    int error_flag, port;
    char ip[50];
    struct sockaddr_in server_address;
    int address_len;

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
    } else {
        printf("connect\n");
    }
}    

Client:: ~Client()
{
    printf("Goodbye\n");
    shutdown(socket_fd, 2);
    close(socket_fd);
}

int Client:: send_message(char *buffer, int buf_len)
{
    int error_flag; 
///!!!!  handler is needed
    error_flag = write(socket_fd, buffer, buf_len);
/*    if (error_flag == (-1)) {
        perror("write");
        exit(errno);
    } else if (error_flag == 0) {
        return (-1);
    }*/
    buffer[0] = 0;
    return error_flag;
}

int Client:: get_message(char *buffer, int buf_len)
{
    int error_flag;
    error_flag = read(socket_fd, buffer, buf_len);
    if (error_flag == -1) {
        perror("read");
        exit(errno);
    }
    buffer[error_flag] = 0;
    error_flag = fputs(buffer, stdout);
    if (error_flag == EOF) {
        return -1;
    }
    buffer[0] = 0;
    return 1;
}
