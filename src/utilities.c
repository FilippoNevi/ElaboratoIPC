#include "utilities.h"

int ** creaMatrice(int ordine) {
	int **matrice, i;

	matrice = calloc(ordine, sizeof(int *));

	for(i = 0; i < ordine; i++)
		matrice[i] = calloc(ordine, sizeof(int));

	return matrice;
}

void leggiMatrice(int fileMatrice, int ** matrice, int ordine) {

char * buff;		// Buffer su cui salvare il contenuto di ogni riga
int n;				// Numero di byte letti dalla funzione read
int i = 0, j = 0, k, x = 0;
char * temp;		// Array temporaneo su cui salvo ogni numero che leggo

	buff = malloc(DIM_BUFF * sizeof(char));
	temp = malloc(DIM_BUFF * sizeof(char));

	while((n = leggiRiga(fileMatrice, buff)) > 0) {

		buff[n] = '\0';

		for(k = 0; k < sizeof(buff) && (buff[k] != '\0'); k++) {

			if((buff[k] != ' ') && (buff[k] != '\n')) {
				temp[x] = buff[k];
				x++;
			}
			else {
				temp[x+1] = '\0';
				x = 0;
				matrice[i][j] = atoi(temp);

				if(buff[k] == '\n') {	// Se sono arrivato a fine riga passo alla successiva
					i++;
					j = 0;
				}
				else {					// Altrimenti vado alla prossima colonna della riga
					j++;
				}
			}
		}
	}
}

int leggiRiga(int fileMatrice, char * buff) {
char carattere;			// Carattere su cui salvo ogni byte che leggo dal file
int dim, dimTot = 0;
	
	if((dim = read(fileMatrice, &carattere, 1)) > 0) {	// Leggo un byte alla volta
		if((carattere == EOF) || (carattere == '\n')){
			return 0;
		}
		else {
			dimTot++;
			
			while((carattere != '\n') && (carattere != EOF) && (dim > 0)) {
				*buff = carattere;
				buff++;
				dim = read(fileMatrice, &carattere, 1);
				dimTot++;
			}

			return dimTot;
		}
	}
	else {
		return 0;
	}
}