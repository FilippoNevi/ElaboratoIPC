#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int main() {
char str[15];
int i, j;
int *array;
int **matrice;

	for(i = 0; i < 15; i++) {
		str[i] = 1;
	}

	strcpy(str, "PROVA123");
	for(i = 0; i < 15; i++) {
		printf("%d ",str[i]);
	}

	printf("\n\n\n");

	array = malloc(10 * sizeof(int));
	for(i = 0; i < 10; i++) {
		array[i] = 40 + i;
		printf("%d ", array[i]);
	}

	printf("\n\n\n");

	matrice = malloc(10 * sizeof(int *));
	for(i = 0; i < 10; i++) {
		matrice[i] = malloc(10 * sizeof(int));
	}

	for(i = 0; i < 3; i++) {
		for(j = 0; j < 3; j++) {
			matrice[i][j] = 60 + i + j;
		}
	}

	for(i = 0; i < 3; i++) {
		for(j = 0; j < 3; j++) {
			printf("%d ", matrice[i][j]);
		}
		printf("\n");
	}
}