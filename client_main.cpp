#include "client.h"

int main(void) {
    int error_flag; 
    char buffer[MAX_BUF];
    FILE *input;
    Client client;
    
    input = fopen("input.txt", "r");
    while(fgets(buffer, MAX_BUF, input)) {
        error_flag = client.send_message(buffer, strlen(buffer));
        if (error_flag == (-1)) {
            perror("send_message");
            exit(errno);
        } else if (error_flag == 0) {
            printf("Server has gone");
            break;
        }
    }
    fclose(input);
    return 0;
}
