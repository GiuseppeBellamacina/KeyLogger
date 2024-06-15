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

#define PORT 12345
#define BUFFER_SIZE 1024

void error(const char *msg) {
    perror(msg);
    exit(EXIT_FAILURE);
}

void send_file(const char *filename, const char *ip_address) {
    FILE *file = fopen(filename, "rb");
    if (file == NULL) {
        error("Error opening file");
    }

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

    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(ip_address);
    server_addr.sin_port = htons(PORT);

    if (connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) == SOCKET_ERROR) {
        error("Error connecting to server");
    }

    char buffer[BUFFER_SIZE];
    size_t bytes_read;
    while ((bytes_read = fread(buffer, 1, BUFFER_SIZE, file)) > 0) {
        if (send(sockfd, buffer, bytes_read, 0) == SOCKET_ERROR) {
            error("Error sending file");
        }
    }

    fclose(file);
    closesocket(sockfd);

    #ifdef _WIN32
    WSACleanup();
    #endif
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <filename> <ip_address>\n", argv[0]);
        return EXIT_FAILURE;
    }

    const char *filename = argv[1];
    const char *ip_address = argv[2];

    send_file(filename, ip_address);

    printf("File sent successfully.\n");

    return EXIT_SUCCESS;
}
