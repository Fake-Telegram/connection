#include "client.h"

int main(void) {
    int error_flag; 
    char buffer[MAX_BUF + 1];
//    FILE *input;
    Client client;
    int max_fd;
//    int pipe_fd[2];
    fd_set read_fds;
    
    /*error_flag = pipe(pipe_fd);
    if (error_flag == (-1)) {
        perror("opening pipe");
        exit(errno);
    }
    client.client_fd = pipe_fd[0];
    max_fd = pipe_fd[0] > client.server_fd ? 
            pipe_fd[0] : client.server_fd;*/
    max_fd = client.socket_fd;
    while (1) {
    /*input = fopen("input.txt", "r");
    while(fgets(buffer, MAX_BUF, stdin)) {
        error_flag = write(pipe_fd[1], buffer, strlen(buffer) + 1);
        if (error_flag == (-1)) {
            perror("writing in pipe");
            exit(errno);
        }*/
        /*error_flag = read(pipe_fd[0], buffer, MAX_BUF);
        if (error_flag == (-1)) {
            perror("reading from pipe");
            exit(errno);
        }*/
        FD_ZERO(&read_fds);
        FD_SET(client.socket_fd, &read_fds);
       // FD_SET(pipe_fd[0], &read_fds);
        FD_SET(0, &read_fds);
        error_flag = select(max_fd + 1, &read_fds, NULL, NULL, NULL);
        if (error_flag == (-1)) {
            perror("select");
            exit(errno);
        }
        if (FD_ISSET(0, &read_fds)) {
            if (!fgets(buffer, MAX_BUF, stdin)) {
                break;
            }
            error_flag = client.send_message(buffer, strlen(buffer) + 1);
            if (error_flag == (-1)) {
                perror("send_message");
                exit(errno);
            } else if (error_flag == 0) {
                printf("Server has gone");
                break;
            }
        }
        if (FD_ISSET(client.socket_fd, &read_fds)) {
            error_flag = client.get_message(buffer, MAX_BUF);
            if (error_flag == (-1)) {
                perror("get_message");
                exit(errno);
            } 
        }            
    }
    //fclose(input);
    return 0;
}
