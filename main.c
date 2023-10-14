#include <stdio.h>
#include <unistd.h>
char str[128];
void retstr(char* st){
	printf("%s",st);
}
int main(int argc, char* argv[]){
	while (fgets(str, sizeof(str), stdin) != 0 && *str != '\n') {
		processline(str);
	}
	clear_deftab();
	clear_nametab();
	return 0;
}
