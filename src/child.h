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

#ifndef CHILD_H
#define CHILD_H


/**
  * @brief Funzione che esegue il comando assegnato dal padre
  *
  * @param pipe Descriptor della pipe tramite la quale il padre scrive al figlio
  *
  */
void eseguiComando(int pipe);

#endif