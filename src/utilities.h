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

#ifndef UTILITIES_H
#define UTILITIES_H

#define DIM_BUFF 512	/// Numero di byte massimo che può occupare una riga della matrice

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
 * @param dimBuff Indica la dimensione massima del buffer
 *
 * @return Numero di byte letti
 */
int leggiRiga(int fileMatrice, char * buff, int dimBuff);

/*@} */

#endif