#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <errno.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/time.h>
#define MAX_QUEUE 5
#define MAX_BUF 1024

int make_socket(int port, char *address)
{
    int socket_fd, error_flag, aux;
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
    aux = 1;
    error_flag = setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR,
            &aux, sizeof(aux));
    if (error_flag == -1) {
        perror("setsockopt");
        exit(errno);
    }
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
        listening_socket, 
        client_fd,
        address_len,
        num_ch,
        port, i, j, 
        max_fd, 
        num_fd;
    struct sockaddr_in client_address;
    char buffer[MAX_BUF], address[50];
    FILE *parameters, *output;
    int client_fds[1000];
    fd_set read_fds;

    parameters = fopen("parameters.txt", "r");
    if (fscanf(parameters, "%d%s", &port, address) != 2) {
        perror("Error in parameters.txt");
        exit(errno);
    }
    fclose(parameters);
    output = fopen("output.txt", "w");

    listening_socket = make_socket(port, address);
    error_flag = listen(listening_socket, MAX_QUEUE);
    if (error_flag == -1) {
        perror("listen");
        exit(errno);
    }
    num_fd = 0;
    do {
        max_fd = listening_socket;
        FD_ZERO(&read_fds);
        FD_SET(listening_socket, &read_fds);
        for (i = 0; i < num_fd; i++) {
            FD_SET(client_fds[i], &read_fds);
            if (client_fds[i] > max_fd) {
                max_fd = client_fds[i];
            }
        }
        error_flag = select(max_fd + 1, &read_fds, NULL, NULL, NULL);
        if (error_flag == -1) {
            perror("select");
            exit(errno);
        }
        if  (FD_ISSET(listening_socket, &read_fds)) {
            address_len = sizeof(client_address);
            client_fd = accept(listening_socket,
                    (struct sockaddr*)&client_address,
                    (socklen_t*)&address_len);
            if (client_fd == -1) {
                perror("accept");
                exit(errno);
            }
            client_fds[num_fd] = client_fd;
            num_fd++;
            fprintf(output, "%s : %d connected\n", 
                    inet_ntoa(client_address.sin_addr),
                    ntohs(client_address.sin_port));
            error_flag = fflush(output);
            if (error_flag) {
                perror("flush");
                exit(errno);
            }
        }
        for (i = 0; i < num_fd; i++) {
            if (FD_ISSET(client_fds[i], &read_fds)) {
                num_ch = read(client_fds[i], buffer, 
                        sizeof(buffer) - 1);
                if (num_ch == -1) {
                    perror("read");
                    exit(errno);
                }
                if (num_ch == 0) {
                    fprintf(output, "%d gone\n", client_fds[i]);
                    error_flag = fflush(output);
                    if (error_flag) {
                        perror("flush");
                        exit(errno);
                    }
                    FD_CLR(client_fds[i], &read_fds);
                    shutdown(client_fds[i], 2);
                    close(client_fds[i]);
                    for (j = i + 1; j < num_fd; j++) {
                        client_fds[j - 1] = client_fds[j];
                    }
                    num_fd--;
                } else {
                    buffer[num_ch] = 0;
                    fprintf(output, "%d %s\n", client_fds[i], buffer);
                    error_flag = fflush(output);
                    if (error_flag) {
                        perror("flush");
                        exit(errno);
                    }
                }
            }
        }
//	printf("num_fd = %d\n", num_fd);
//    } while (num_fd != 0);
    } while (1);
    for (i = 0; i < num_fd; i++) {
        close(client_fds[i]);
    }
    close (listening_socket);
    fclose(output);
    return 0;
}
        


