//
// Created by xosel on 19/06/2019.
//

#ifndef MCGRUDER_TYPES_H
#define MCGRUDER_TYPES_H

// Llibreries
#include <signal.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <time.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// constants de protocol de comunicacio
//      CONNECTION
//          --- TYPE ---
#define TYPE_CONNECTION 0x01
//#define TYPE_CONNECTION '1'
//          --- HEADER ---
#define HEADER_CONNECTION "[]"
#define HEADER_CONNECTION_RESPONSE_OK "[CONOK]"
#define HEADER_CONNECTION_RESPONSE_KO "[CONKO]"

//      DISCONNECTION
//          --- TYPE ---
#define TYPE_DISCONNECTION 0x02
//          --- HEADER ---
#define HEADER_DISCONNECTION "[]"
#define HEADER_DISCONNECTION_RESPONSE_OK "[CONOK]"
#define HEADER_DISCONNECTION_RESPONSE_KO "[CONKO]"

//      SEND FILE
//          --- TYPE ---
#define TYPE_SENDFILE 0x03
//          --- HEADER ---
#define HEADER_SENDFILE_METADATA "[METADATA]"
#define HEADER_SENDFILE_DATA "[]"
#define HEADER_SENDFILE_ENDFILE "[ENDFILE]"
#define HEADER_SENDFILE_RESPONSE_OK_TEXT "[FILEOK]"
#define HEADER_SENDFILE_RESPONSE_KO_TEXT "[FILEKO]"
#define HEADER_SENDFILE_RESPONSE_OK_IMAGE "[CHECKOK]"
#define HEADER_SENDFILE_RESPONSE_KO_IMAGE "[CHECKKO]"


#define LOCALTIME (+2)

//Tipus propi: Configuracio
typedef struct {
    char* telescopeName;
    int time;
    char* ip;
    int port;
}Configuracio;

//Tipus propi: Trama
typedef struct {
    char type;
    char* header;
    short length;
    char* data;
}Trama;

typedef struct{
    int dia;
    int mes;
    int any;
}Data;

typedef struct{
    int hora;
    int minut;
}Hora;

Configuracio configuracio;
int fdLionel;
struct sockaddr_in socketConfig;
int mcGruderStatus;
char* pathAcumulat;

#endif //MCGRUDER_TYPES_H
