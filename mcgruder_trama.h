//
// Created by xosel on 02/07/2019.
//

#ifndef MCGRUDER_MCGRUDER_TRAMA_H
#define MCGRUDER_MCGRUDER_TRAMA_H

#include "mcgruder_screen.h"


/**
 * Metode per a enviar de forma generica una trama
 * @param trama : trama a enviar
 * @return int : 0 si tot OK / 1 si hi ha hagut error (Lionel ha caigut)
 */
int sendTrama(Trama trama);

/**
 * Funcio que envia la trama de desconnexio a Lionel cridant la funcio 'sendTrama' amb els valors requerits
 * @return int : 0 si tot OK / 1 si hi ha hagut error (Lionel ha caigut)
 */
int enviaTramaDesconnexio();

/**
 * Funcio que retorna la trama llegida (Lionel->McGruder)
 */
Trama receiveTrama();

/**
 * Enviar i rebre la trama de connexio
 * @return 1 Error / 0 OK
 */
int sendConnectionTrama();

#endif //MCGRUDER_MCGRUDER_TRAMA_H
