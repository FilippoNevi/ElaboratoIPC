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

#include  "utilities.h"

#ifndef CHILD_H
#define CHILD_H

/// Modalità moltiplicazione
#define MOLTIPLICA 0

/// Modalità somma
#define SOMMA 1

/// Dimensione massima comando da leggere (i, j, k, e ordine possono avere massimo 3 cifre)
#define DIM_COM 13+1


/**
  * @brief Funzione che esegue il comando assegnato dal padre
  *
  * @param pipe Descriptor della pipe tramite la quale il padre scrive al figlio
  *
  */
void leggiComando(int pipe);

/**
  *
  *
  *
  *
  */
void eseguiComando(int comando, int riga, int colonna, int ordine);

#endif