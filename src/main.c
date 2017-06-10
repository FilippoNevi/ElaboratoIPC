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
int ** matProcessi;									// Matrice per la gestione dei processi
int fileA, fileB, fileC;							// Descriptor dei file delle matrici
int chiusuraFlag = 0;								// Flag per tener traccia di errori riscontrati
int memA, memB, memC, memSomma;						// Riferimenti alle aree di memoria condivise
int i, j;
int * matCondA, * matCondB, * matCondC, sommaCond;	// Dati condivisi dai processi
char * bufferOutput = (char *) malloc(256 * sizeof(char));
int semaforo;										// Semaforo che gestisce la memoria condivisa
struct sembuf op;									// Operazione da applicare al semaforo

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

	/// Creazione delle quattro aree di memoria condivisa
	memA = shmget(SHM_KEY_A, sizeof(int[ordine][ordine]), (0666 | IPC_CREAT));	// IPC_CREAT crea una nuova entry se key non esiste
	memB = shmget(SHM_KEY_B, sizeof(int[ordine][ordine]), (0666 | IPC_CREAT));
	memC = shmget(SHM_KEY_C, sizeof(int[ordine][ordine]), (0666 | IPC_CREAT));
	memSomma = shmget(SHM_KEY_SOMMA, sizeof(int), (0666 | IPC_CREAT));

	if((memA == -1) || (memB == -1) || (memC == -1) || (memSomma == -1)) {
		strcpy(bufferOutput, "Errore: creazione della memoria condivisa non riuscita.");
		write(STDOUT, bufferOutput, sizeof(bufferOutput));
		return 0;
	}

	matCondA = shmat(memA, NULL, 0);
	if(matCondA == (void *)-1) {
		strcpy(bufferOutput, "Errore: impossibile effettuare attach della matrice A.");
		write(STDOUT, bufferOutput, sizeof(bufferOutput));
		return 0;
	}

	matCondB = shmat(memB, NULL, 0);
	if(matCondB == (void *)-1) {
		strcpy(bufferOutput, "Errore: impossibile effettuare attach della matrice B.");
		write(STDOUT, bufferOutput, sizeof(bufferOutput));
		return 0;
	}

	matCondC = shmat(memC, NULL, 0);
	if(matCondC == (void *)-1) {
		strcpy(bufferOutput, "Errore: impossibile effettuare attach della matrice C.");
		write(STDOUT, bufferOutput, sizeof(bufferOutput));
		return 0;
	}

	sommaCond = shmat(memSomma, NULL, 0);
	if(sommaCond == (void *)-1) {
		strcpy(bufferOutput, "Errore: impossibile effettuare attach della somma di C.");
		write(STDOUT, bufferOutput, sizeof(bufferOutput));
		return 0;
	}

	*sommaCond = 0;

	semaforo = semget(SEM_KEY, 1, 0666 | IPC_CREAT | IPC_EXCL);
	if(semaforo < 0) {
		strcpy(bufferOutput, "Errore: impossibile creare il semaforo.");
		write(STDOUT, bufferOutput, sizeof(bufferOutput));
		return 0;
	}

	// sem_num = 0 perché c'è solo un semaforo, e lo setto a 1 in modo da usarlo come mutex
	op.sem_num = 0;
	op.sem_op = 1;
	op.sem_flg = 0;

	// Applico op a semaforo (1 = numero di operazioni da applicare)
	semop(semaforo, &op, 1);

	
}