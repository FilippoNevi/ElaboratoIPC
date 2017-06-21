/**
  * @file child.h
  *
  * @brief Funzioni che esegue il processo figlio
  * per gestire i comandi assegnati dal padre ed
  * eseguirli.
  *
  * @author Filippo Nevi VR398146
  *
  */

#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/types.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/msg.h>

#include  "utilities.h"

#ifndef CHILD_H
#define CHILD_H

typedef struct {
  long mtype;
  int comando;
  int riga;
  int colonna;
  int pid;
}messaggio;


/**
  * @brief Funzione che elabora il comando assegnato dal padre e chiama eseguiComando
  *
  * @param pipe Descriptor della pipe tramite la quale il padre scrive al figlio
  *
  */
void leggiComando(int pipe);

/**
  * @brief Funzione che esegue il comando inviato dal padre
  *
  * @param comando Operazione da eseguire
  *
  * @param riga Riga della matrice A (in caso di moltiplicazione)
  * o C (in caso di somma) da usare
  *
  * @param colonna Colonna della matrice B (in caso di moltiplicazione,
  * -1 altrimenti) da usare
  * 
  * @param ordine Ordine delle matrici
  *
  */
void eseguiComando(int comando, int riga, int colonna, int ordine);

#endif