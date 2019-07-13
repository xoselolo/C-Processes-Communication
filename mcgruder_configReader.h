//
// Created by xosel on 19/06/2019.
//

#ifndef MCGRUDER_CONFIGREADER_H
#define MCGRUDER_CONFIGREADER_H

// Llibreries
#include <fcntl.h>
#include <stdlib.h>
#include "mcgruder_screen.h"

/**
 * Funcio que retorna el file descriptor associat al arxiu de configuracio
 * del mcGruder o en cas d'error (no trobar l'arxiu o no poder-lo obrir
 * retorna 0
 */
int existsFile(char* path);

/**
 * Funcio que retorna la configuracio de l'arxiu de configuracio
 * o bé NULL en cas de que l'arxiu sigui erroni
 */
Configuracio readConfiguracio(int fd);

/**
 * Funció que llegeix una cadena de caràcters fins a un caràcter específivc a través d'un fd
 * @param fd        : File Descriptor pel que efectuem la lectura
 * @param limit     : Caràcter que limitarà la cadena
 * @param cadena    : Cadena (per referència) que desarem
 * @return Integer:
 *      0 : S'ha llegit correctament
 *     -1 : S'ha aturat la lectura sobtadament (s'han llegit 0 bytes)
 */
int llegirCadena(int fd, char limit, char *cadena);

#endif //MCGRUDER_CONFIGREADER_H
