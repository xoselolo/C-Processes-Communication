//
// Created by xosel on 20/06/2019.
//

#ifndef MCGRUDER_CONNECTION_H
#define MCGRUDER_CONNECTION_H

// Llibreries
#include "mcgruder_memory.h"
#include "mcgruder_trama.h"

// Capceleres de funcions
/**
 * Funcio que retorna 0 si la connexio s'ha establert amb exit
 * i que retorna 1 si la connexio no s'ha pogut establir
 */
int connecta();

/**
 * Funcio que desconnecta el McGruder de Lionel enviant trama de desconnexio i esperant a la resposta de Lionel
 */
void desconnecta();

/**
 * Metode que desconnecta McGruder del Lionel tancant el FD i alliberant memoria
 */
void desconnectaForsaBruta();


#endif //MCGRUDER_CONNECTION_H
