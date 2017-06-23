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

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/types.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/msg.h>
#include <sys/wait.h>

#include "utilities.h"
#include "child.h"

#define NUM_PARAMETRI 5+1	// Argomenti (5) + nome eseguibile (1)

int main(int argc, char *argv[]) {

int ** matA, ** matB, ** matC, ordine, numFigli;	// Parametri del programma
int ** matProcessi;									// Matrice per la gestione dei processi
int fileMatA, fileMatB, fileMatC;							// Descriptor dei file delle matrici
int chiusuraFlag = 0;								// Flag per tener traccia di errori riscontrati
int memA, memB, memC, memSomma;						// Riferimenti alle aree di memoria condivise
int i, j, k;
int * matCondA, * matCondB, * matCondC, *sommaCond;	// Dati condivisi dai processi
int semaforo;										// Semaforo che gestisce la memoria condivisa
struct sembuf op;									// Operazione da applicare al semaforo
int **pipeComandi;									// Matrice di pipe per comunicare ai figli i comandi da svolgere
int *pidFigli;										// Array di pid dei figli
int codaMessaggi;
int pid;
char * buff;
messaggio msgFiglio;
int processiLiberi;
int messaggiRicevuti;
int processiAttivi;
int indicePipe;
int trovato;


	if(argc != NUM_PARAMETRI) {
		segnala("Errore: i parametri devono essere matriceA matriceB matriceC ordine processiFiglio.\n\n");
		chiusuraFlag = 1;
	}

	if((fileMatA = open(argv[1], O_RDONLY, S_IRUSR)) < 0) {	// Apertura file della matrice A
		segnala("Errore: impossibile aprire il file della matrice A.\n\n");
		chiusuraFlag = 1;
	}

	if((fileMatB = open(argv[2], O_RDONLY, S_IRUSR)) < 0) {	// Apertura file della matrice B
		segnala("Errore: impossibile aprire il file della matrice B.\n\n");
		chiusuraFlag = 1;
	}

	if((fileMatC = creat(argv[3], O_RDWR | O_TRUNC | S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH)) < 0) {
		segnala("Errore: impossibile creare il file della matrice C.\n\n");
		chiusuraFlag = 1;
	}

	if((ordine = atoi(argv[4])) <= 0) {
		segnala("Errore: l'ordine delle matrici deve essere maggiore di zero.\n\n");
		chiusuraFlag = 1;
	}

	if((numFigli = atoi(argv[5])) <= 0) {
		segnala("Errore: il numero dei processi figli deve essere maggiore di zero.\n\n");
		chiusuraFlag = 1;
	}

	if(chiusuraFlag) {
		close(fileMatA);
		close(fileMatB);
		close(fileMatC);

		return 0;
	}

	pipeComandi = malloc(numFigli * sizeof(int *));
	if(pipeComandi == NULL) {
		segnala("Errore: impossibile creare la matrice di pipe.\n\n");
		close(fileMatA);
		close(fileMatB);
		close(fileMatC);
		return 0;
	}
	for(i = 0; i < numFigli; i++) {
		pipeComandi[i] = malloc(sizeof(int[2]));
	}

	matA = creaMatrice(ordine);
	matB = creaMatrice(ordine);
	matC = creaMatrice(ordine);

	matProcessi = creaMatrice(ordine);

	leggiMatrice(fileMatA, matA, ordine);
	leggiMatrice(fileMatB, matB, ordine);

	/// Creazione delle quattro aree di memoria condivisa
	memA = shmget(SHM_KEY_A, sizeof(int[ordine][ordine]), (0666 | IPC_CREAT));	// IPC_CREAT crea una nuova entry se key non esiste
	memB = shmget(SHM_KEY_B, sizeof(int[ordine][ordine]), (0666 | IPC_CREAT));
	memC = shmget(SHM_KEY_C, sizeof(int[ordine][ordine]), (0666 | IPC_CREAT));
	memSomma = shmget(SHM_KEY_SOMMA, sizeof(int), (0666 | IPC_CREAT));

	if((memA == -1) || (memB == -1) || (memC == -1) || (memSomma == -1)) {
		segnala("Errore: creazione della memoria condivisa non riuscita.\n\n");
		free(pipeComandi);
		close(fileMatA);
		close(fileMatB);
		close(fileMatC);
		return 0;
	}

	matCondA = shmat(memA, NULL, 0);
	if(matCondA == (void *)-1) {
		segnala("Errore: impossibile effettuare attach della matrice A.\n\n");
		free(pipeComandi);
		close(fileMatA);
		close(fileMatB);
		close(fileMatC);
		return 0;
	}

	matCondB = shmat(memB, NULL, 0);
	if(matCondB == (void *)-1) {
		segnala("Errore: impossibile effettuare attach della matrice B.\n\n");
		free(pipeComandi);
		close(fileMatA);
		close(fileMatB);
		close(fileMatC);
		return 0;
	}

	matCondC = shmat(memC, NULL, 0);
	if(matCondC == (void *)-1) {
		segnala("Errore: impossibile effettuare attach della matrice C.\n\n");
		free(pipeComandi);
		close(fileMatA);
		close(fileMatB);
		close(fileMatC);
		return 0;
	}

	sommaCond = shmat(memSomma, NULL, 0);
	if(sommaCond == (void *)-1) {
		segnala("Errore: impossibile effettuare attach della somma di C.\n\n");
		free(pipeComandi);
		close(fileMatA);
		close(fileMatB);
		close(fileMatC);
		return 0;
	}

	caricaMatrice(matA, matCondA, ordine);
	caricaMatrice(matB, matCondB, ordine);

	*sommaCond = 0;

	semaforo = semget(SEM_KEY, 1, 0666 | IPC_CREAT | IPC_EXCL);
	if(semaforo < 0) {
		segnala("Errore: impossibile creare il semaforo.\n\n");
		free(pipeComandi);
		close(fileMatA);
		close(fileMatB);
		close(fileMatC);
		return 0;
	}

	// sem_num = 0 perché c'è solo un semaforo, e lo setto a 1 in modo da usarlo come mutex
	op.sem_num = 0;
	op.sem_op = 1;
	op.sem_flg = 0;

	// Applico op a semaforo (1 = numero di operazioni da applicare)
	semop(semaforo, &op, 1);

	pidFigli = malloc(numFigli * sizeof(int));
	if(pidFigli == NULL) {
		segnala("Errore: impossibile creare i processi figlio.\n\n");
		free(pipeComandi);
		close(fileMatA);
		close(fileMatB);
		close(fileMatC);
		return 0;
	}

	codaMessaggi = msgget(MSG_KEY, 0666 | IPC_CREAT | IPC_EXCL);
	if(codaMessaggi == -1) {
		segnala("Errore: impossibile creare la coda dei messaggi.\n\n");
		free(pipeComandi);
		free(pidFigli);
		close(fileMatA);
		close(fileMatB);
		close(fileMatC);
		return 0;
	}

	/// Creazione figli
	for(i = 0; i < numFigli; i++) {
		/// Instauro la pipe
		if(pipe(pipeComandi[i]) == -1) {
			segnala("Errore: impossibile creare la pipe.\n\n");
			free(pidFigli);
			free(pipeComandi);
			freeMatrice(matA, ordine);
			freeMatrice(matB, ordine);
			freeMatrice(matC, ordine);
			freeMatrice(matProcessi, ordine);
			close(fileMatA);
			close(fileMatB);
			close(fileMatC);
			return 0;
		}

		/// Creo i figli
		pid = fork();
		if(pid == -1) {
			segnala("Errore: impossibile creare il processo figlio.\n\n");
			free(pidFigli);
			free(pipeComandi);
			freeMatrice(matA, ordine);
			freeMatrice(matB, ordine);
			freeMatrice(matC, ordine);
			freeMatrice(matProcessi, ordine);
			close(fileMatA);
			close(fileMatB);
			close(fileMatC);
			return 0;
		}
		else if(pid == 0) {				// Codice processo figlio
			if(close(pipeComandi[i][1]) == -1) {	// Chiudo la pipe di scrittura del figlio
				segnala("Errore: impossibile chiudere la pipe di scrittura nel child.\n\n");
				free(pidFigli);
				free(pipeComandi);
				freeMatrice(matA, ordine);
				freeMatrice(matB, ordine);
				freeMatrice(matC, ordine);
				freeMatrice(matProcessi, ordine);
				close(fileMatA);
				close(fileMatB);
				close(fileMatC);
				exit(1);
			}

			leggiComando(pipeComandi[i][0]);

			if(close(pipeComandi[i][0]) == -1) {	// Chiudo la pipe di scrittura del figlio
				segnala("Errore: impossibile chiudere la pipe di lettura nel child.\n\n");
				free(pidFigli);
				free(pipeComandi);
				freeMatrice(matA, ordine);
				freeMatrice(matB, ordine);
				freeMatrice(matC, ordine);
				freeMatrice(matProcessi, ordine);
				close(fileMatA);
				close(fileMatB);
				close(fileMatC);
				exit(1);
			}

			freeMatrice(matA, ordine);
			freeMatrice(matB, ordine);
			freeMatrice(matC, ordine);
			freeMatrice(matProcessi, ordine);
			close(fileMatA);
			close(fileMatB);
			close(fileMatC);
			exit(0);
		}
		else {							// Codice padre
			pidFigli[i] = pid;
			close(pipeComandi[i][0]);
		}
	}

	segnala("Ho creato tutti i processi figlio.\n\n");

	if((buff = malloc(sizeof(char) * DIM_COM)) == NULL) {
		segnala("Errore: impossibile allocare il buffer del comando.\n\n");
		free(pidFigli);
		free(pipeComandi);
		freeMatrice(matA, ordine);
		freeMatrice(matB, ordine);
		freeMatrice(matC, ordine);
		freeMatrice(matProcessi, ordine);
		close(fileMatA);
		close(fileMatB);
		close(fileMatC);
		return 0;
	}

	segnala("Assegno i comandi ai vari figli.\n\n");
	if(ordine == 1) {

		if(numFigli > 1) {
			// Creo il comando e lo invio al figlio
			creaComando(buff, MOLTIPLICA, 0, 0, ordine);
			write(pipeComandi[0][1], buff, strlen(buff));

			creaComando(buff, SOMMA, 0, -1, ordine);
			write(pipeComandi[1][1], buff, strlen(buff));

			for(i = 0; i < 2; i++) {
				// Mi metto in ascolto della risposta dei figli
				msgrcv(codaMessaggi, &msgFiglio, sizeof(messaggio) - sizeof(long), 1, 0);

				if(msgFiglio.comando == MOLTIPLICA) {
					// Segno che l'unica cella da elaborare è stata completata
					matProcessi[msgFiglio.riga][msgFiglio.colonna] = 1;
				}
			}
		}
		else {
			creaComando(buff, MOLTIPLICA, 0, 0, ordine);
			write(pipeComandi[0][1], buff, strlen(buff));
			
			// Mi metto in ascolto della risposta del figlio
			msgrcv(codaMessaggi, &msgFiglio, sizeof(messaggio) - sizeof(long), 1, 0);

			matProcessi[msgFiglio.riga][msgFiglio.colonna] = 1;

			creaComando(buff, SOMMA, 0, -1, ordine);
			write(pipeComandi[0][1], buff, strlen(buff));

			// Mi metto in ascolto della risposta del figlio
			msgrcv(codaMessaggi, &msgFiglio, sizeof(messaggio) - sizeof(long), 1, 0);

		}
	}
	else if((ordine * ordine) <= numFigli) {

		// Faccio partire le moltiplicazioni
		for(i = 0; i < ordine; i++) {
			for(j = 0; j < ordine; j++) {
				creaComando(buff, MOLTIPLICA, i, j, ordine);
				write(pipeComandi[(i * ordine) + j][1], buff, strlen(buff));
			}
		}

		segnala("Ho fatto partire le moltiplicazioni.\n\n");

		// Ricevo i messaggi dai figli a cui ho assegnato un'operazione
		for(i = 0; i < ordine * ordine; i++) {

			// Mi metto in ascolto della risposta di un figlio
			msgrcv(codaMessaggi, &msgFiglio, sizeof(messaggio) - sizeof(long), 1, 0);

			// Segno che la cella assegnata è stata completata
			matProcessi[msgFiglio.riga][msgFiglio.colonna] = 1;
		}

		// Faccio partire le somme, una volta che le moltiplicazioni sono finite
		for(i = 0; i < ordine; i++) {
			creaComando(buff, SOMMA, i, -1, ordine);
			write(pipeComandi[i][1], buff, strlen(buff));
		}

		segnala("Ho fatto partire tutte le somme.\n\n");

		for(i = 0; i < ordine; i++)
			msgrcv(codaMessaggi, &msgFiglio, sizeof(messaggio) - sizeof(long), 1, 0);

	}
	else {
		int operazioniTotali = (ordine * ordine);			// Numero di operazioni totali da compiere
		int count = 0, operazioniEffettuate, prossimaRiga, prossimaColonna;
		int finito = 0;												// Flag che indica se sono stati fatti partire tutti i processi possibili

		for(i = 0; i < ordine && !finito; i++) {
			for(j = 0; j < ordine && !finito; j++) {
				if(count <= numFigli) {
					creaComando(buff, MOLTIPLICA, i, j, ordine);
					write(pipeComandi[(i * ordine) + j][1], buff, strlen(buff));
					count++;
				}
				else {
					finito = 1;
					prossimaRiga = i + 1;
					prossimaColonna = j + 1;
				}
			}
		}

		// Procedo con le restanti moltiplicazioni
		while(operazioniEffettuate <= (ordine * ordine)) {
			msgrcv(codaMessaggi, &msgFiglio, sizeof(messaggio) - sizeof(long), 1, 0);

			// Cerco il pid del figlio che ha terminato
			for(j = 0; j < numFigli && !trovato; j++) {
				if(pidFigli[j] == msgFiglio.pid) {
					indicePipe = j;
					trovato = 1;
				}
			}

			creaComando(buff, MOLTIPLICA, prossimaRiga, prossimaColonna, ordine);
			write(pipeComandi[indicePipe][1], buff, strlen(buff));

			prossimaRiga++;
			prossimaColonna++;
			operazioniEffettuate++;
		}

		i = 0;								// Indice della riga da sommare

		// Effettuo le somme
		while(operazioniEffettuate <= operazioniTotali) {
			msgrcv(codaMessaggi, &msgFiglio, sizeof(messaggio) - sizeof(long), 1, 0);

			// Cerco il pid del figlio che ha terminato
			for(j = 0; j < numFigli && !trovato; j++) {
				if(pidFigli[j] == msgFiglio.pid) {
					indicePipe = j;
					trovato = 1;
				}
			}

			creaComando(buff, SOMMA, msgFiglio.riga, -1, ordine);
			write(pipeComandi[indicePipe][1], buff, strlen(buff));

			i++;
			operazioniEffettuate++;
		}
	}

	segnala("Sto chiudendo tutti i processi.\n\n");
	creaComando(buff, ESCI, -1, -1, ordine);
	for(i = 0; i < numFigli; i++) {
		write(pipeComandi[i][1], buff, strlen(buff));
	}

	for(i = 0; i < ordine; i++) {
		for(j = 0; j < ordine; j++) {
			matC[i][j] = matCondC[(i * ordine) + j];
		}
	}

	segnala("Sto scrivendo su file la matrice C.\n\n");
	for(i = 0; i < ordine; i++) {
		for(j = 0; j < ordine; j++) {
			sprintf(buff, "%d ", matC[i][j]);
			write(fileMatC, buff, strlen(buff));
		}
		sprintf(buff, "\n");
		write(fileMatC, buff, strlen(buff));
	}

	sprintf(buff, "La somma delle righe della matrice C e': %d.\n\n", *sommaCond);
	segnala(buff);


	// In attesa che ogni figlio termini
	while(wait(NULL) > 0);


	// Detach delle matrici condivise
	if((shmdt(matCondA)) < 0)
		segnala("Errore: impossibile effettuare detach della matrice A.\n\n");

	if((shmdt(matCondB)) < 0)
		segnala("Errore: impossibile effettuare detach della matrice B.\n\n");

	if((shmdt(matCondC)) < 0)
		segnala("Errore: impossibile effettuare detach della matrice C.\n\n");

	if((shmdt(sommaCond)) < 0)
		segnala("Errore: impossibile effettuare detach della somma condivisa.\n\n");


	// Chiusura delle aree di memoria condivisa
	if((shmctl(memA, IPC_RMID, NULL)) < 0)
		segnala("Errore: impossibile cancellare l'area di memoria di A.\n\n");

	if((shmctl(memB, IPC_RMID, NULL)) < 0)
		segnala("Errore: impossibile cancellare l'area di memoria di B.\n\n");

	if((shmctl(memC, IPC_RMID, NULL)) < 0)
		segnala("Errore: impossibile cancellare l'area di memoria di C.\n\n");

	if((shmctl(memSomma, IPC_RMID, NULL)) < 0)
		segnala("Errore: impossibile cancellare l'area di memoria della somma.\n\n");


	// Chiusura coda messaggi e semaforo
	if((msgctl(codaMessaggi, IPC_RMID, NULL)) < 0)
		segnala("Errore: impossibile chiudere la coda di messaggi.\n\n");

	if((semctl(semaforo, 0, IPC_RMID)) < 0)
		segnala("Errore: impossibile chiudere il semaforo.\n\n");


	// Free della pipe
	for(i = 0; i < numFigli; i++) {
		free(pipeComandi[i]);
	}

	free(pidFigli);
	//free(buff);
	freeMatrice(matA, ordine);
	freeMatrice(matB, ordine);
	freeMatrice(matC, ordine);
	freeMatrice(matProcessi, ordine);
	close(fileMatA);
	close(fileMatB);
	close(fileMatC);

	segnala("Ho deallocato ogni risorsa e chiuso ogni struttura.\n\n");

	return 0;
}