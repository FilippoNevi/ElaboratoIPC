#include "child.h"

void leggiComando(int pipe) {
char buff[DIM_COM];				// Buffer su cui salvo il comando di cui fare parsing
int comando, riga, colonna, ordine;		// Parametri del comando inviato dal padre
int i, j;
char temp[3+1];					// Stringa temporanea su cui salvo i numeri da trasformare in int
int esci = 0;

	while(!esci) {

		if(read(pipe, &buff, DIM_COM) > 0) {
			// Parsing del comando
			if(buff[0] == MOLTIPLICA) {				// Devo moltiplicare

				comando = MOLTIPLICA;
				for(i = 2, j = 0; buff[i] != ' '; i++, j++)
					temp[j] = buff[i];
				temp[j] = '\0';
				riga = atoi(temp);

				i++;			// Sposto i alla cella dove inizia il numero della colonna
				for(j = 0; buff[i] != ' '; i++, j++)
					temp[j] = buff[i];
				temp[j] = '\0';
				colonna = atoi(temp);

				i++;
				for(j = 0; i < DIM_COM && buff[i] != '\0'; i++, j++)
					temp[j] = buff[i];
				temp[j] = '\0';
				ordine = atoi(temp);

				segnala("Child: ho ricevuto il comando di moltiplicare la riga %d di A e la colonna %d di B.\n\n", riga, colonna);
				eseguiComando(comando, riga, colonna, ordine);
			}

			else if(buff[0] == SOMMA) {				// Devo sommare

				comando = SOMMA;
				for(i = 2, j = 0; buff[i] != ' '; i++, j++)
					temp[j] = buff[i];
				temp[j] = '\0';
				riga = atoi(temp);

				i++;
				for(j = 0; i < DIM_COM && buff[i] != '\0'; i++, j++)
					temp[j] = buff[i];
				temp[j] = '\0';
				ordine = atoi(temp);

				colonna = -1;

				segnala("Child: ho ricevuto il comando di sommare la riga %d di C.\n\n", riga);
				eseguiComando(comando, riga, colonna, ordine);
			}

			else if(buff[0] == ESCI){				// Devo uscire
				segnala("Child: ho ricevuto il comando di terminare l'esecuzione.\n\n");
				esci = 1;
			}

		}
		
	}
}

void eseguiComando(int comando, int riga, int colonna, int ordine) {
int semaforo, codaMessaggi;
struct sembuf op;
int memA, memB, memC, memSomma;
int * matCondA, * matCondB, * matCondC, * sommaCond;
int * tempA, * tempB, risultato;
	
	if((semaforo = semget(SEM_KEY, 1, 0666)) == -1) {
		segnala("Errore: impossibile creare il semaforo nel child.\n\n")
	}

	if((codaMessaggi = msgget(MSG_KEY, 0666)) == -1) {
		segnala("Errore: impossibile creare la coda di messaggi nel child.\n\n");
	}


	if((memA = shmget(SHM_KEY_A, sizeof(int[ordine][ordine]), 0666)) == -1) {
		segnala("Errore: impossibile ricevere riferimento all'area di memoria condivisa della matrice A nel child.\n\n");
	}

	if((memB = shmget(SHM_KEY_B, sizeof(int[ordine][ordine]), 0666)) == -1) {
		segnala("Errore: impossibile ricevere riferimento all'area di memoria condivisa della matrice B nel child.\n\n");
	}

	if((memC = shmget(SMH_KEY_C, sizeof(int[ordine][ordine]), 0666)) == -1) {
		segnala("Errore: impossibile ricevere riferimento all'area di memoria condivisa della matrice C nel child.\n\n");
	}

	if((memSomma = shmget(SHM_KEY_SOMMA, sizeof(int), 0666)) == -1) {
		segnala("Errore: impossibile riceve riferimento all'area di memoria condivisa della variabile somma nel child.\n\n");
	}


	if((matCondA = shmat(memA, NULL, 0)) == (void *)-1) {
		segnala("Errore: impossibile effettuare l'attach della matrice A nel child.\n\n");
	}

	if((matCondB = shmat(memB, NULL, 0)) == (void *)-1) {
		segnala("Errore: impossibile effettuare l'attach della matrice B nel child.\n\n");
	}

	if((matCondC = shmat(memC, NULL, 0)) == (void *)-1) {
		segnala("Errore: impossibile effettuare l'attach della matrice C nel child.\n\n");
	}

	if((sommaCond = shmat(memSomma, NULL, 0)) == (void *)-1) {
		segnala("Errore: impossibile effettuare l'attach della somma di C nel child.\n\n");
	}


	if(comando == MOLTIPLICA) {	
	// Nella moltiplicazione il semaforo non è richiesto, ogni cella di C viene calcolata da un solo processo
	int i, j;

		// Salvo la riga e la colonna da moltiplicare in due array
		if((tempA = malloc(ordine * sizeof(int))) == -1) {
			segnala("Errore: impossibile allocare vettore temporaneo durante l'esecuzione di un child.\n\n");
			exit(1);
		}
		if((tempB = malloc(ordine * sizeof(int))) == -1) {
			segnala("Errore: impossibile allocare vettore temporaneo durante l'esecuzione di un child.\n\n");
			exit(1);
		}

		// Ricavo la riga di A da moltiplicare
		j = 0;
		for(i = ordine * riga; i < ordine * (riga+1); i++, j++) {
			tempA[j] = matCondA[i];
		}

		// Ricavo la colonna di B da moltiplicare
		j = 0;
		for(i = 0; i < ordine; i++, j++) {
			tempB[j] = matCondB[(i * ordine) + colonna];
		}

		// Calcolo il risultato della moltiplicazione
		risultato = 0;
		for(i = 0; i < ordine; i++)
			risultato += ((tempA[i]) * (tempB[i]));

		// Scrivo il risultato sulla matrice C condivisa
		matCondC[(riga * ordine) + colonna] = risultato;

		// Notifico al padre che ho finito l'operazione richiesta
		messaggio msg;

		msg.mtype = 1;
		msg.comando = comando;
		msg.riga = riga;
		msg.colonna = colonna;
		msg.pid = getpid();

		msgsnd(codaMessaggi, &msg, sizeof(msg) - sizeof(long), 1);

		free(tempA);
		free(tempB);
	}

	if(comando == SOMMA) {
	int i;

		risultato = 0;

		// Calcolo la somma della riga di C
		for(i = 0; i < ordine; i++)
			risultato += matCondC[(riga * ordine) + i];

		// Richiedo il semaforo
		op.sem_num = 0;
		op.sem_op = -1;
		op.sem_flg = 0;
		semop(semaforo, &op, 1);

		// Incremento la variabile condivisa
		*sommaCond += risultato;

		// Rilascio il semaforo
		op.sem_op = 1;
		semop(semaforo, &op, 1);

		// Notifico al padre che ho finito l'operazione richiesta
		messaggio msg;

		msg.mtype = 1;
		msg.comando = comando;
		msg.riga = riga;
		msg.colonna = colonna;
		msg.pid = getpid();

		msgsnd(codaMessaggi, &msg, sizeof(msg) - sizeof(long), 1);
	}

	if((shmdt(matCondA)) < 0)
		segnala("Errore: impossibile effettuare detach della matrice A nel child.\n\n");

	if((shmdt(matCondB)) < 0)
		segnala("Errore: impossibile effettuare detach della matrice B nel child.\n\n");

	if((shmdt(matCondC)) < 0)
		segnala("Errore: impossibile effettuare detach della matrice C nel child.\n\n");

	if((shmdt(sommaCond)) < 0)
		segnala("Errore: impossibile effettuare detach della somma condivisa nel child.\n\n");

}