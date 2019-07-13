//
// Created by xosel on 02/07/2019.
//

#ifndef MCGRUDER_LIONEL_TRAMA_H
#define MCGRUDER_LIONEL_TRAMA_H

#include "lionel_types.h"
#include "lionel_screen.h"

void mostraTrama(Trama connectionTrama);

/**
 * Funcio que envia una trama al macgruder amb @param fd especificat
 * @return 0 OK / 1 McGruder ha caigut
 */
int sendTrama(Trama trama, int fd);
Trama receiveTrama(int fd);

/**
 * Funcio que tracta la trama rebuda per un mcgruder amb el @param fd
 * @return
 *      0   :   OK
 *      1   :   McGruder ha caigut
 *      2   :   Hem enviat i rebut trama de desconnexio
 *      3   :   Trama desconeguda
 *      4   :   Tipus d'extensio incorrecte
 *      5   :   Fitxer enviat correctament
 *      6   :   Error al enviar el fitxer
 */
int tractaTrama(Trama received, int fd);

/**
 * Funcio per a identificar el tipus de extensio de l'arxiu que estem rebent
 */
int extensioMetadata(char* receivedData);

/**
 * Funcio per a obtenir les dades del camp data de la metadata
 * Recordem el format del camp data de la trama de metadata
 * DATA: "[__EXTENSIO__&__MIDAFITXER__&__DATARECEPCIO__&__NOMFITXER__]"
 */
Image getImageInfo(Trama received);

#endif //MCGRUDER_LIONEL_TRAMA_H
