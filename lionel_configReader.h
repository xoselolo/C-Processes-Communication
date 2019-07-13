//
// Created by xosel on 25/06/2019.
//

#ifndef MCGRUDER_LIONEL_CONFIGREADER_H
#define MCGRUDER_LIONEL_CONFIGREADER_H

// Llibreries
#include <fcntl.h>
#include "lionel_screen.h"

// Capceleres de funcions
/**
 * Funcio que retorna el file descriptor associat al arxiu de configuracio
 * del mcGruder o en cas d'error (no trobar l'arxiu o no poder-lo obrir)
 * retorna 0
 */
int existsFile(char* path);

/**
 * Funcio per a obtenir la configuracio de Lionel del fitxer passat com a paramtre
 * en l'execucio del programa
 */
Configuracio readConfiguracio(int fdConfig);

/**
 * Funcio per a llegir una cadena fins a trobar el caracter especificat
 */
int llegirCadena(int fd, char limit, char *cadena);

#endif //MCGRUDER_LIONEL_CONFIGREADER_H
