//
// Created by xosel on 25/06/2019.
//

#ifndef MCGRUDER_LIONEL_START_H
#define MCGRUDER_LIONEL_START_H

// Llibreries
#include "lionel_configReader.h"
#include "lionel_connection.h"

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

/**
 * Crea y configura el socket. Retorna:
 *      0 : tot ok
 *      1 : error
 */
int configure();

/**
 * Configura la variable global on guardarem els mcgruders que tenim connectats
 *  (array i num de mcgruders connectats)
 */
void configureInitialMcGruders();

/**
 * Configura la variable global on guardarem les imatges i els fitxers de dades astronomiques rebudes pels mcgruders
 */
void configureInitialFiles();

/**
 * Configura les senyals de destruccio del proces
 */
void configureKills();


#endif //MCGRUDER_LIONEL_START_H
