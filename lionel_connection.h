//
// Created by xosel on 27/06/2019.
//

#ifndef MCGRUDER_LIONEL_CONNECTION_H
#define MCGRUDER_LIONEL_CONNECTION_H

#include "lionel_screen.h"
#include "lionel_memory.h"
#include "lionel_trama.h"
#include <signal.h>

void desconnecta();
void killMcGruders();

/**
 * Metode per a crear un nou McGruder i desar-lo a l'estructura a partir del seu @param fdNewMcgruder
 */
void creaNouMcGruder(int fdNewMcgruder);

/**
 * Afegeix el @param mcgruder a l'estructura
 */
void addNewMcGruder(Mcgruder mcgruder);

/**
 * Retorna l'index del mcgruder que te com a fd el fd passat com a parametre.
 * En cas d'error retornara:
 *      -1 : No hi ha McGruders a l'estructura
 *      -2 : No s'ha trobat el McGruder amb FD igual a @param fdMcGruder
 */
int indexOfMcGruder(int fdMcGruder);

/**
 * Elimina el McGruder amb fd igual a @param index
 * Retorna
 *      -1 : si no s'ha trobat el mcgruder
 *      1  : si s'ha eliminat
 */
int deleteMcGruder(int fdMcGruder, int indexKnown);

/**
 * Posa com a nom del mcgruder
 */
void posaNomMcGruder(char* nom, int fdMcgruder);


// Funcio del thread del mcgruder
void * mcGruderFunc(void* arg);

/**
 * Un Mcgruder amb FD @param fd ha caigut, l'hem d'eliminar
 */
void mcGruderDisconnectedElimination(int fd);

#endif //MCGRUDER_LIONEL_CONNECTION_H
