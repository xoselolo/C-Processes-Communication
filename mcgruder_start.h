//
// Created by xosel on 19/06/2019.
//

#ifndef MCGRUDER_START_H
#define MCGRUDER_START_H

// Llibreries
#include "mcgruder_configReader.h"
#include "mcgruder_connection.h"


/**
 * Funcio que retorna -1 si hi ha hagut algun error
 * i retorna el FD associat a l'arxiu en cas de que l'arrencada ha estat satisfactoria
 * Arrencada:
 *      control d'arguments
 *      lectura del fitxer
 *      configuracio
 */
int start(int argc, char** argv);

/**
 * Funcio que retornara un 0 en cas de que el numero d'arguments sigui correcte
 * @param argc : Numero d'arguments
 * @return 0 si ok / 1 si ko
 */
int controlaNumeroArguments(int argc);

// todo
void configure();


#endif //MCGRUDER_START_H
