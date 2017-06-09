/**
  * @file main.c
  *
  * @brief Funzione main, che gestisce i parametri passati dall'utente,
  * legge i dati dai file, inizializza le aree di memoria condivisa e
  * sincronizza i processi figli per poter moltiplicare le matrici A e B
  * (dello stesso ordine) e fare la somma della matrice C risultante.
  *
  *
  * @author Filippo Nevi VR398146
  *
  */

#include <stdlib.h>
#include <fcntl.h>

#include "utilities.h"

#define NUM_PARAMETRI 5+1	// Argomenti (5) + nome eseguibile (1)
#define STDOUT 1

int main(int argc, char *argv[]) {

int ** matA, ** matB, ** matC, ordine, numFigli;	// Parametri del programma
int ** matProcessi;								// Matrice per la gestione dei processi
int fileA, fileB, fileC;						// Descriptor dei file delle matrici
int chiusuraFlag = 0;							// Flag per tener traccia di errori riscontrati
int i, j;

char * bufferOutput = (char *) malloc(256 * sizeof(char));

	/**
	  * @brief Apertura e creazione dei file di input e output
	*/
	if(argc != NUM_PARAMETRI) {
		strcpy(bufferOutput, "Errore: i parametri devono essere matriceA matriceB matriceC ordine processiFiglio.");
		write(STDOUT, bufferOutput, sizeof(bufferOutput));
		chiusuraFlag = 1;
	}

	if((fileMatA = open(argv[1], O_RDONLY, S_IRUSR)) < 0) {	// Apertura file della matrice A
		strcpy(bufferOutput, "Errore: impossibile aprire il file della matrice A.");
		write(STDOUT, bufferOutput, sizeof(bufferOutput));
		chiusuraFlag = 1;
	}

	if((fileMatB = open(argv[2], O_RDONLY, S_IRUSR)) < 0) {	// Apertura file della matrice B
		strcpy(bufferOutput, "Errore: impossibile aprire il file della matrice B.");
		write(STDOUT, bufferOutput, sizeof(bufferOutput));
		chiusuraFlag = 1;
	}

	if((fileMatC = creat(argv[3], O_RDWR | O_TRUNC | S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH)) < 0) {
		strcpy(bufferOutput, "Errore: impossibile creare il file della matrice C.");
		write(STDOUT, bufferOutput, sizeof(bufferOutput));
		chiusuraFlag = 1;
	}

	if((ordine = atoi(argv[4])) <= 0) {
		strcpy(bufferOutput, "Errore: l'ordine delle matrici deve essere maggiore di zero.");
		write(STDOUT, bufferOutput, sizeof(bufferOutput));
		chiusuraFlag = 1;
	}

	if((numFigli = atoi(argv[5])) <= 0) {
		strcpy(bufferOutput, "Errore: il numero dei processi figli deve essere maggiore di zero.");
		write(STDOUT, bufferOutput, sizeof(bufferOutput));
		chiusuraFlag = 1;
	}

	if(chiusuraFlag) {
		close(fileMatA);
		close(fileMatB);
		close(fileMatC);
		free(bufferOutput);

		return 0;
	}

	matA = crea_matrice(ordine);
	matB = crea_matrice(ordine);
	matC = crea_matrice(ordine);

	matProcessi = crea_matrice(ordine);

	leggiMatrice(fileMatA, matA, ordine);
	leggiMatrice(fileMatB, matB, ordine);
}