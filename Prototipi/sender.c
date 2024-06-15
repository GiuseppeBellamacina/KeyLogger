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

void send_file(const char *filename, const char *ip_address) {
    FILE *file = fopen(filename, "rb");
    if (file == NULL) {
        error("Error opening file");
    }

    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        error("Error creating socket");
    }

    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(ip_address);
    server_addr.sin_port = htons(PORT);

    if (connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        error("Error connecting to server");
    }

    char buffer[BUFFER_SIZE];
    size_t bytes_read;
    while ((bytes_read = fread(buffer, 1, BUFFER_SIZE, file)) > 0) {
        if (send(sockfd, buffer, bytes_read, 0) == -1) {
            error("Error sending file");
        }
    }

    fclose(file);
    close(sockfd);
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
