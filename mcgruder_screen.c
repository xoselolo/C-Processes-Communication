//
// Created by xosel on 19/06/2019.
//
// Llibreries
#include "mcgruder_screen.h"

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
    char buff[100];
    int msg;
    msg = sprintf(buff, "Starting %s", configuracio.telescopeName);
    buff[msg++] = '\n';
    write(1, buff, msg);
}


void mostraMissatgeConnecting(){
    write(FD_OUT, MSSG_CONNECTING, strlen(MSSG_CONNECTING) * sizeof(char));
}

void mostraMissatgeConnectionReady(){
    write(FD_OUT, MSSG_CONNREADY, strlen(MSSG_CONNREADY) * sizeof(char));
}

void mostraMissatgeConnectionFailed(){
    write(FD_OUT, MSSG_CONFAILED, strlen(MSSG_CONFAILED) * sizeof(char));
}

void mostraMissatgeWaiting(){
    write(FD_OUT, MSSG_WAITING, strlen(MSSG_WAITING) * sizeof(char));
}

void mostraMissatgeDisconnecting(){
    /*char buff[100];
    int msg;
    msg = sprintf(buff, "\nDisconnecting %s\n", configuracio.telescopeName);
    buff[msg++] = '\n';
    write(1, buff, msg);*/

    write(FD_OUT, "Disconnecting ", strlen("Disconnecting ") * sizeof(char));
    write(FD_OUT, configuracio.telescopeName, strlen(configuracio.telescopeName) * sizeof(char));
    write(FD_OUT, "\n", sizeof(char));
}

void mostraErrorOberturaDirectori(char* dirPath){
    write(FD_OUT, MSSG_ERROR_OPERTURA_DIRECTORI, strlen(MSSG_ERROR_OPERTURA_DIRECTORI) * sizeof(char));
    write(FD_OUT, dirPath, strlen(dirPath) * sizeof(char));
    write(FD_OUT, "\n", sizeof(char));
}

void mostraMissatgeTestingFiles(){
    write(FD_OUT, MSSG_TESTING_FILES, strlen(MSSG_TESTING_FILES) * sizeof(char));
}

void mostraMissatgeExploringDirectory(char* dirPath){
    write(FD_OUT, "Exploring ", strlen("Exploring ") * sizeof(char));
    write(FD_OUT, dirPath, strlen(dirPath) * sizeof(char));
    write(FD_OUT, "...\n", strlen("...\n") * sizeof(char));
}

void mostraMissatgeFileName(char* filename){
    write(FD_OUT, "File: ", 6 * sizeof(char));
    write(FD_OUT, filename, strlen(filename) * sizeof(char));
    write(FD_OUT, "\n", sizeof(char));
}

void mostraMissatgeSendingFile(char* filename){
    write(FD_OUT, "Sending: ", 9 * sizeof(char));
    write(FD_OUT, filename, strlen(filename) * sizeof(char));
    write(FD_OUT, "...\n", 4 * sizeof(char));
}

void mostraMissatgeFileSent(){
    write(FD_OUT, "File sent.\n", 12 * sizeof(char));
}

void mostraErrorEnviarFitxer(char* filename){
    char buff[100];
    int msg;
    msg = sprintf(buff, "Error on sending file '%s'\n", filename);
    write(FD_OUT, buff, msg);
}

void mostraErrorConnexio(){
    write(FD_OUT, MSSG_ERROR_CONNECTION, strlen(MSSG_ERROR_CONNECTION) * sizeof(char));
}

void mostraMissatgePercentatge(int totalBytesRead, int size, char* filename){
    float percentatge = (totalBytesRead/(float)size)*100;
    char buff[100];
    int msg;
    msg = sprintf(buff, "\tFile '%s' %.2f%c  sent. \n", filename, percentatge, '%');
    write(FD_OUT, buff, msg);
}