#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 12345
#define BUFFER_SIZE 1024

void error(const char *msg) {
    perror(msg);
    exit(EXIT_FAILURE);
}

void receive_file() {
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        error("Error creating socket");
    }

    struct sockaddr_in server_addr, client_addr;
    socklen_t client_len = sizeof(client_addr);

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    if (bind(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        error("Error binding socket");
    }

    if (listen(sockfd, 5) == -1) {
        error("Error listening on socket");
    }

    printf("Server listening on port %d...\n", PORT);

    int client_sockfd = accept(sockfd, (struct sockaddr *)&client_addr, &client_len);
    if (client_sockfd == -1) {
        error("Error accepting connection");
    }

    printf("Client connected.\n");

    FILE *file = fopen("received_file", "wb");
    if (file == NULL) {
        error("Error creating file");
    }

    char buffer[BUFFER_SIZE];
    ssize_t bytes_received;
    while ((bytes_received = recv(client_sockfd, buffer, BUFFER_SIZE, 0)) > 0) {
        if (fwrite(buffer, 1, bytes_received, file) < bytes_received) {
            error("Error writing to file");
        }
    }

    if (bytes_received == -1) {
        error("Error receiving data");
    }

    fclose(file);
    close(client_sockfd);
    close(sockfd);

    printf("File received successfully.\n");
}

int main() {
    receive_file();
    return EXIT_SUCCESS;
}
