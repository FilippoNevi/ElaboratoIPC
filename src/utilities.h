/**
  * @file utilities.h
  * 
  * @brief Libreria di funzioni e costanti usate per gestire i dati
  * del programma.
  * 
  * @author Filippo Nevi VR398146
  * 
  */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>


#ifndef UTILITIES_H
#define UTILITIES_H

/// Costante dello standard output, per la stampa a schermo
#define STDOUT 1

/// Dimensione della variabile buffer
#define DIM_BUFF 512

/// Chiave della memoria condivisa della matrice A
#define SHM_KEY_A 80

/// Chiave della memoria condivisa della matrice B
#define SHM_KEY_B 81

/// Chiave della memoria condivisa della matrice C
#define SHM_KEY_C 82

/// Chiave della memoria condivisa della somma di C
#define SHM_KEY_SOMMA 83

/// Chiave del semaforo
#define SEM_KEY 150

/// Chiave della coda di messaggi
#define MSG_KEY 200

/// Dimensione massima comando da leggere (i, j, k, e ordine possono avere massimo 3 cifre)
#define DIM_COM 13+1

/// Moltiplicazione di una riga di A con una colonna di B
#define MOLTIPLICA 0

/// Somma di una riga di C
#define SOMMA 1

/// Chiusura del child
#define ESCI 2

/** @name Funzioni per la gestione delle matrici */
/*@{ */

/**
  * @brief Funzione che crea una matrice quadrata dell'ordine fornito
  *
  * @param ordine Ordine della matrice 
  *
  * @return Puntatore alla matrice creata
  *
  */
int ** creaMatrice(int ordine);

/**
 * @brief Funzione che legge da un file una matrice e la salva
 *
 * @param fileMatrice File descriptor della matrice da leggere
 *
 * @param matrice Puntatore alla matrice su cui salvare i dati
 *
 */
void leggiMatrice(int fileMatrice, int ** matrice);

/**
 * @brief Funzione che legge riga per riga un file
 *
 * @param fileMatrice File descriptor della matrice da cui leggere 
 *
 * @param buff Buffer su cui salvare la riga letta
 *
 * @return Numero di byte letti
 *
 */
int leggiRiga(int fileMatrice, char * buff);

/**
  * @brief Funzione che carica nell'area di memoria condivisa la matrice
  *
  * @param matrice Puntatore alla variabile matrice da caricare
  *
  * @param matriceCond Puntatore all'area di memoria condivisa
  *
  * @param ordine Ordine della matrice da caricare
  *
  */
void caricaMatrice(int ** matrice, int * matriceCond, int ordine);


/**
  * @brief Funzione che effettua la free di una matrice
  *
  * @param matrice Puntatore alla variabile matrice
  *
  * @param ordine Ordine della matrice
  *
  */
void freeMatrice(int ** matrice, int ordine);

/*@} */


/**
  * @brief Funzione che stampa a schermo una stringa
  *
  * @param bufferOutput Stringa su cui viene salvato il testo
  *
  */
void segnala(char * bufferOutput);


/**
  * @brief Funzione che crea la stringa da inviare tramite pipe
  * ad ogni processo figlio
  *
  * @param buff Puntatore al buffer su cui salvare il comando generato
  *
  * @param comando Operazione che deve eseguire il figlio
  *
  * @param riga Riga della matrice A (in caso di moltiplicazione)
  * o C (in caso di somma)
  *
  * @param colonna Colonna della matrice B (in caso di moltiplicazione)
  * 
  * @param ordine Ordine delle matrici
  *
  */
void creaComando(char * buff, int comando, int riga, int colonna, int ordine);

#endif