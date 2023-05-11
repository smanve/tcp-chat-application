// Including necessary header files for sockets, networking, and standard I/O
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <unistd.h>

// Here we define server IP, port number, and buffer size constants
#define SERVER_IP "172.23.181.164" 
#define PORT 5100
#define BUFFER_SIZE 1024
#define EXIT_CMD "exit\n"

// Entry point of the program
int main() {
    // Define variables and server socket address structure
    int client_fd;
    struct sockaddr_in server_addr; 
    char buffer[BUFFER_SIZE];
    fd_set read_fds;

    // Creation of a new socket
    client_fd = socket(AF_INET, SOCK_STREAM, 0); 
    if (client_fd == -1) {
        perror("socket");
        exit(EXIT_FAILURE); 
    }

    // Configuring server address settings
    server_addr.sin_family = AF_INET; 
    server_addr.sin_addr.s_addr = inet_addr(SERVER_IP); 
    server_addr.sin_port = htons(PORT);

    // Connecting to the server
    if (connect(client_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) { 
        perror("connect");
        exit(EXIT_FAILURE);
    }

    // While loop which sends and receives messages, as while(1) is always true, this loop keeps running until it encounters a break statement
    while (1) {
        FD_ZERO(&read_fds);
        FD_SET(STDIN_FILENO, &read_fds);
        FD_SET(client_fd, &read_fds);

        if (select(client_fd + 1, &read_fds, NULL, NULL, NULL) == -1) {
            perror("select");
            exit(EXIT_FAILURE);
        }

        if (FD_ISSET(client_fd, &read_fds)) {
            // Receive a message from the server
            memset(buffer, 0, BUFFER_SIZE); 
            if (read(client_fd, buffer, BUFFER_SIZE) <= 0) {
                perror("read");
                break;
            }
            printf("Server: %s", buffer);

            if (strcmp(buffer, EXIT_CMD) == 0) {
                printf("Server exited. Client will now exit.\n");
                break;
            }
        }

        if (FD_ISSET(STDIN_FILENO, &read_fds)) {
            // Send a message to the server
            printf("Client: ");
            fgets(buffer, BUFFER_SIZE, stdin); 
            if (send(client_fd, buffer, strlen(buffer), 0) <= 0) {
                perror("send");
                break;
            }

            if (strcmp(buffer, EXIT_CMD) == 0) {
                printf("Client will now exit.\n");
                break;
            }
        }
    }

    // Close the client socket
    close(client_fd);
    return 0; 
}
