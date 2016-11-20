#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <errno.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/time.h>
#include <vector>
#include <list>
#include <set>
#define MAX_QUEUE SOMAXCONN
#define MAX_BUF 1024

using namespace std;

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

void close_client(set<int> &client_fds, int fd) {
        set<int>::iterator it;

        it = client_fds.find(fd);
        shutdown(fd, 2);
        close(fd);
        client_fds.erase(it);
        //say his friends, that he is gone
}

void behave(int fd, char *buffer, int buf_len, set<int> &client_fd) 
{
    int error_flag;
    int name;
    error_flag = read(fd, buffer, buf_len);
    if (error_flag == -1) {
        throw ("read");
        return;
    }
    if (error_flag == 0) {
        close_client(client_fd, fd);
        return;
    }
	printf("%s\n",buffer);
    error_flag = sscanf(buffer, "%d", &name);
    if (error_flag != 1) {
        error_flag = write(fd, "receiver is not mentioned\n", 30);
        if (error_flag == 0) {
            close_client(client_fd, fd);
            return;
        }
        if (error_flag == -1) {
            throw ("write");
        }
    } else {
        //write message in data base
        if (client_fd.find(name) != client_fd.end()) {
            error_flag = write(name, buffer, buf_len);
            if (error_flag == -1) {
                throw("write");
            }
            if (error_flag == 0) {
                close_client(client_fd, name);
                return;
            } 
        } else {
            return;
        }
    } 
    error_flag = write(fd, "sent\n", 30);
    if (error_flag == 0) {
        close_client(client_fd, fd);
        return;
    }
    if (error_flag == -1) {
        throw ("write");
    }
    return;
}


int main(void) {
    int error_flag, 
        listening_socket, 
        client_fd,
        address_len,
        port,
        max_fd; 
    struct sockaddr_in client_address;
    char buffer[MAX_BUF + 1], address[50];
    FILE *parameters, *output;
    set<int> client_fds;
    set<int>::iterator s_it;
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
try{
    do {
        max_fd = listening_socket;
        FD_ZERO(&read_fds);
        FD_SET(listening_socket, &read_fds);
        for (s_it = client_fds.begin();
        s_it != client_fds.end(); s_it++) {
            FD_SET(*s_it, &read_fds);
            if (*s_it > max_fd) {
                max_fd = *s_it;
            }
        }
        error_flag = select(max_fd + 1, &read_fds, NULL, NULL, NULL);
        if (error_flag == -1) {
            throw("select");
            exit(errno);
        }
        if  (FD_ISSET(listening_socket, &read_fds)) {
            address_len = sizeof(client_address);
            client_fd = accept(listening_socket,
                    (struct sockaddr*)&client_address,
                    (socklen_t*)&address_len);
            if (client_fd == -1) {
                throw("accept");
            }
            client_fds.insert(client_fd);
            fprintf(output, "%s : %d connected in %d\n", 
                    inet_ntoa(client_address.sin_addr),
                    ntohs(client_address.sin_port),
                    client_fd);
            fflush(output);
        }
        for (s_it = client_fds.begin(); 
        s_it != client_fds.end(); s_it++) {
            if (FD_ISSET(*s_it, &read_fds)) {
                behave(*s_it, buffer, MAX_BUF, client_fds);
                buffer[0] = 0;
            }
        }
    } while (1);
}   catch(const char* s) {
        perror(s);
    }
    for (s_it = client_fds.begin(); 
    s_it != client_fds.end(); s_it++) {
        close(*s_it);
    }
    close (listening_socket);
    fclose(output);
    return 0;
}
