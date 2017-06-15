#include <stdio.h>
#include <string.h>

int main() {
char str[15];
int i;

	for(i = 0; i < 15; i++) {
		str[i] = 1;
	}

	strcpy(str, "PROVA123");
	for(i = 0; i < 15; i++) {
		printf("%d ",str[i]);
	}
}