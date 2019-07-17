//
// Created by xosel on 19/06/2019.
//

#ifndef MCGRUDER_SCREEN_H
#define MCGRUDER_SCREEN_H

// Llibreries
#include "mcgruder_types.h"

// Constants
// Constants numeriques
#define FD_OUT 1
// Constants cadenes
#define MSSG_ERROR_NUMERO_ARGUMENTS "Error, número d'arguments incorrecte!\n"
#define MSSG_ERROR_FITXER_NO_TROBAT "Error, no s'ha pogut trobar l'arxiu!\n"
#define MSSG_ERROR_FORMAT_FITXER_CONFIGURACIO "Error, format de l'arxiu de configuracio incorrecte!\n"
#define MSSG_ERROR_OPERTURA_DIRECTORI "Error, no s'ha pogut obrir el directori "
#define MSSG_ERROR_CONNECTION "Error, no s'ha pogut establir connexio amb Lionel!\n"
#define MSSG_ERROR_FILE_TYPE "Error, tipus de fitxer no valid!\n"

#define MSSG_CONNECTING "Connecting to Lionel...\n"
#define MSSG_CONNREADY "Connection ready.\n"
#define MSSG_CONFAILED "Connection failed.\n"
#define MSSG_WAITING "Waiting...\n"
#define MSSG_TESTING_FILES "Testing files...\n"


// Capcaleres de funcions
void mostraErrorNumeroArguments();
void mostraErrorFitxerConfiguracioNoTrobat();
void mostraErrorFormatArxiuConfiguracio();
void mostraMissatgeStarting();
void mostraMissatgeConnecting();
void mostraMissatgeConnectionReady();
void mostraMissatgeConnectionFailed();
void mostraMissatgeWaiting();
void mostraMissatgeDisconnecting();
void mostraErrorOberturaDirectori(char* dirPath);
void mostraMissatgeTestingFiles();
void mostraMissatgeExploringDirectory(char* dirPath);
void mostraMissatgeFileName(char* filename);
void mostraMissatgeSendingFile(char* filename);
void mostraMissatgeFileSent();
void mostraErrorEnviarFitxer(char* filename);
void mostraErrorConnexio();
void mostraMissatgePercentatge(int totalBytesRead, int size, char* filename);
void mostraErrorTipusDeFitxer();

#endif //MCGRUDER_SCREEN_H
