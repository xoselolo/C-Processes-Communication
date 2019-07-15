//
// Created by xosel on 25/06/2019.
//

#include "lionel_screen.h"

// Semafors globals
pthread_mutex_t mutexLlistaMcGruders = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutexLlistaImatges = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutexLlistaTxts = PTHREAD_MUTEX_INITIALIZER;

// Funcions
void mostraErrorNumeroArguments(){
    write(FD_OUT, MSSG_ERROR_NUMERO_ARGUMENTS, strlen(MSSG_ERROR_NUMERO_ARGUMENTS) * sizeof(char));
}
void mostraErrorFitxerConfiguracioNoTrobat(){
    write(FD_OUT, MSSG_ERROR_FITXER_NO_TROBAT, strlen(MSSG_ERROR_FITXER_NO_TROBAT) * sizeof(char));
}

void mostraErrorFormatArxiuConfiguracio(){
    write(FD_OUT, MSSG_ERROR_FORMAT_FITXER_CONFIGURACIO, strlen(MSSG_ERROR_FORMAT_FITXER_CONFIGURACIO) * sizeof(char));
}

void mostraMissatgeStarting(){
    write(FD_OUT, MSSG_STARTING, strlen(MSSG_STARTING) * sizeof(char));
}

void mostraMissatgeWaiting(){
    write(FD_OUT, MSSG_WAITING, strlen(MSSG_WAITING) * sizeof(char));
}

void mostraMissatgeConnectionReady(char* mcGruderName){


    write(FD_OUT, "Connection Lionel-", strlen("Connection Lionel-") * sizeof(char));
    write(FD_OUT, mcGruderName, (strlen(mcGruderName) - 1) * sizeof(char));
    write(FD_OUT, " ready\n", strlen(" ready\n") * sizeof(char));
}

void mostraErrorStartConnection(){
    write(FD_OUT, MSSG_ERROR_START_CONNECTION, strlen(MSSG_ERROR_START_CONNECTION) * sizeof(char));
}

void mostraMissatgeDisconnecting(){
    write(FD_OUT, MSSG_DISCONNECTING, strlen(MSSG_DISCONNECTING) * sizeof(char));
}

void mostraMissatgeDisconnectingMcGruder(char* telescopeName){
    char buffer[100];
    int bytes = sprintf(buffer, "Disconnecting from %s \n", telescopeName);
    write(FD_OUT, buffer, (unsigned int) bytes);

    //write(FD_OUT, "Disconnecting from ", strlen("Disconnecting from ") * sizeof(char));
    //write(FD_OUT, telescopeName, strlen(telescopeName) * sizeof(char));
    //write(FD_OUT, ".\n", strlen(".\n") * sizeof(char));
    //write(FD_OUT, ".", sizeof(char));
}

void mostraErrorNewMcgruder(){
    write(FD_OUT, MSSG_ERROR_NEW_MCGRUDER_CONNECTION, strlen(MSSG_ERROR_NEW_MCGRUDER_CONNECTION) * sizeof(char));
}

void mostraErrorConnexioSocket(){
    write(FD_OUT, MSSG_ERROR_SOCKET, strlen(MSSG_ERROR_SOCKET) * sizeof(char));
}

void mostraErrorRebreArxiu(char* filename){
    write(FD_OUT, "Error en la transmissio del fitxer ", strlen("Error en la transmissio del fitxer ") * sizeof(char));
    write(FD_OUT, filename, strlen(filename) * sizeof(char));
    write(FD_OUT, "\n", sizeof(char));
}

void mostraMissatgeReceivingData(char* mcgruderName){
    write(FD_OUT, "Receiving data from ", strlen("Receiving data from ") * sizeof(char));
    write(FD_OUT, mcgruderName, strlen(mcgruderName) * sizeof(char));
    write(FD_OUT, " ... \n", strlen(" ... \n") * sizeof(char));
}

void mostraMissatgeFileReceived(char* filename){
    write(FD_OUT, "File ", strlen("File ") * sizeof(char));
    write(FD_OUT, filename, strlen(filename) * sizeof(char));
    write(FD_OUT, " received. \n", strlen(" received. \n") * sizeof(char));
}