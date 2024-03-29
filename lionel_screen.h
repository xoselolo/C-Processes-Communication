//
// Created by xosel on 25/06/2019.
//

#ifndef MCGRUDER_LIONEL_SCREEN_H
#define MCGRUDER_LIONEL_SCREEN_H

// Llibreries
#include "lionel_types.h"

// Constants
// Constants numeriques
#define FD_OUT 1
// Constants cadenes
#define MSSG_ERROR_NUMERO_ARGUMENTS "Error, número d'arguments incorrecte!\n"
#define MSSG_ERROR_FITXER_NO_TROBAT "Error, no s'ha pogut trobar l'arxiu!\n"
#define MSSG_ERROR_FORMAT_FITXER_CONFIGURACIO "Error, format de l'arxiu de configuracio incorrecte!\n"
#define MSSG_STARTING "Starting Lionel.\n"
#define MSSG_WAITING "Waiting...\n"
#define MSSG_ERROR_START_CONNECTION "Error, no s'ha pogut connectar Lionel!\n"
#define MSSG_ERROR_NEW_MCGRUDER_CONNECTION "Error, no s'ha pogut connectar un nou McGruder!\n"
#define MSSG_DISCONNECTING "\nDisconnecting Lionel..\n"
#define MSSG_ERROR_SOCKET "Error, no s'ha pogut acceptar una nova connexio!\n"
#define MSSG_RECEIVING_DATA "Receiving data from "
#define MSSG_END " ... \n"
#define MSSG_DISCONNECTING_MCGRUDER "Disconnecting from "

// Capcaleres de funcions
void mostraErrorNumeroArguments();
void mostraErrorFitxerConfiguracioNoTrobat();
void mostraErrorFormatArxiuConfiguracio();
void mostraMissatgeStarting();
void mostraMissatgeWaiting();
void mostraMissatgeConnectionReady(char* mcGruderName);
void mostraErrorStartConnection();
void mostraMissatgeDisconnecting();
void mostraMissatgeDisconnectingMcGruder(char* telescopeName);
void mostraErrorNewMcgruder();
void mostraErrorConnexioSocket();
void mostraErrorRebreArxiu(char* filename);
void mostraMissatgeReceivingData(char* mcgruderName);
void mostraMissatgeFileReceived(char* filename);
void mostraErrorArrencarPaquita();
void mostraErrorCreacioQueue();

#endif //MCGRUDER_LIONEL_SCREEN_H
