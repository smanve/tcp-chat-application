// Including necessary header files for sockets, networking, and standard I/Os
#include<arpa/inet.h>
#include<netinet/in.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/socket.h>
#include<sys/select.h>
#include<unistd.h>

// Here port number and buffer size constants have been defined
#define PORT 5100
#define BUFFER_SIZE 1024
#define EXIT_CMD "exit\n"

// Entry point of the program
int main() {

  // Defining varables, server and client socket address structures
  int server_fd, client_fd, addr_len;
  struct sockaddr_in server_addr, client_addr;
  char buffer[BUFFER_SIZE];
  fd_set read_fds;

  // creation of a socket
  server_fd = socket(AF_INET, SOCK_STREAM, 0);

  if (server_fd == -1) {
    perror("socket");
    exit(EXIT_FAILURE);
  }

  // server address settings being configured
  server_addr.sin_family = AF_INET;
  server_addr.sin_addr.s_addr = INADDR_ANY;
  server_addr.sin_port = htons(PORT);

  // Binding the socket to the server address
  if (bind(server_fd, (struct sockaddr * ) & server_addr, sizeof(server_addr)) == -1) {
    perror("bind");
    exit(EXIT_FAILURE);
  }

  // This part listens for incoming connections
  if (listen(server_fd, 1) == -1) {
    perror("listen");
    exit(EXIT_FAILURE);
  }

  // Here we accept an incoming connection
  addr_len = sizeof(client_addr);

  // Server loop to keep listening for new connections
  while (1) {
    client_fd = accept(server_fd, (struct sockaddr * ) & client_addr, (socklen_t * ) & addr_len);

    if (client_fd == -1) {
      perror("accept");
      continue; // if accept fails, go back to start of the loop to accept new connections
    }

    // while loop which receives and sends messages, as while(1) is always true, this loop keeps running until it encounters a break statement
    while (1) {
      FD_ZERO(&read_fds);
      FD_SET(STDIN_FILENO, &read_fds);
      FD_SET(client_fd, &read_fds);
      
      if (select(client_fd + 1, &read_fds, NULL, NULL, NULL) == -1) {
              perror("select");
              exit(EXIT_FAILURE);
          }
      // Receive a message from the client 
      if (FD_ISSET(client_fd, &read_fds)) {
              memset(buffer, 0, BUFFER_SIZE);
              if (read(client_fd, buffer, BUFFER_SIZE) <= 0) {
                  perror("read");
                  break;
              }
              printf("Client: %s", buffer);
              
              // checks if the client types "exit"
              if (strcmp(buffer, EXIT_CMD) == 0) {
                  printf("Client exited. Server waiting for new connections\n");
                  break;
              }
          }

      // Send a message to the client 
      if (FD_ISSET(STDIN_FILENO, &read_fds)) {
              printf("Server: ");
              fgets(buffer, BUFFER_SIZE, stdin);
              if (send(client_fd, buffer, strlen(buffer), 0) <= 0) {
                  perror("send");
                  break;
              }
              // checks if the server typed "exit", and exits if they did
              if (strcmp(buffer, EXIT_CMD) == 0) {
                  printf("Server will now exit.\n");
                  close(client_fd);
                  close(server_fd);
                  return 0; // Server stops and exits
              }
          }
    }
    // Closing client socket after client exited or an error occurred
    close(client_fd);
  }

  // Closing server socket (this line may never be reached if server is intended to run indefinitely)
  close(server_fd);
  return 0;
}
