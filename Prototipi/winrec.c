#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef _WIN32
#include <winsock2.h>
#pragma comment(lib, "ws2_32.lib")
#else
#include <unistd.h>
#include <arpa/inet.h>
#endif

#define PORT 8000
#define BUFFER_SIZE 1024

void error(const char *msg) {
    perror(msg);
    exit(EXIT_FAILURE);
}

void receive_file() {
    #ifdef _WIN32
    WSADATA wsa_data;
    if (WSAStartup(MAKEWORD(2, 2), &wsa_data) != 0) {
        error("Error initializing Winsock");
    }
    #endif

    SOCKET sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == INVALID_SOCKET) {
        error("Error creating socket");
    }

    struct sockaddr_in server_addr, client_addr;
    int client_len = sizeof(client_addr);

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    if (bind(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) == SOCKET_ERROR) {
        error("Error binding socket");
    }

    if (listen(sockfd, 5) == SOCKET_ERROR) {
        error("Error listening on socket");
    }

    printf("Server listening on port %d...\n", PORT);

    SOCKET client_sockfd = accept(sockfd, (struct sockaddr *)&client_addr, &client_len);
    if (client_sockfd == INVALID_SOCKET) {
        error("Error accepting connection");
    }

    printf("Client connected.\n");

    FILE *file = fopen("received_file", "wb");
    if (file == NULL) {
        error("Error creating file");
    }

    char buffer[BUFFER_SIZE];
    int bytes_received;
    while ((bytes_received = recv(client_sockfd, buffer, BUFFER_SIZE, 0)) > 0) {
        if (fwrite(buffer, 1, bytes_received, file) < bytes_received) {
            error("Error writing to file");
        }
    }

    if (bytes_received == SOCKET_ERROR) {
        error("Error receiving data");
    }

    fclose(file);
    closesocket(client_sockfd);
    closesocket(sockfd);

    #ifdef _WIN32
    WSACleanup();
    #endif

    printf("File received successfully.\n");
}

int main() {
    receive_file();
    return EXIT_SUCCESS;
}
// gcc -o server server.c -lws2_32