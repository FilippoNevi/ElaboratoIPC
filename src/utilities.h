/**
  * @file utilities.h
  * 
  * @brief Libreria di funzioni usate per gestire i dati
  * del programma.
  * 
  * @author Filippo Nevi VR398146
  * 
  */

#include <stdlib.h>
#include <unistd.h>

#ifndef UTILITIES_H
#define UTILITIES_H

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
 * @param ordine Ordine della matrice
 *
 */
void leggiMatrice(int fileMatrice, int ** matrice, int ordine);

/**
 * @brief Funzione che legge riga per riga un file
 *
 * @param fileMatrice File descriptor della matrice da cui leggere 
 *
 * @param buff Buffer su cui salvare la riga letta
 *
 * @return Numero di byte letti
 */
int leggiRiga(int fileMatrice, char * buff);

/*@} */

#endif