//
// Created by xosel on 25/06/2019.
//

#ifndef MCGRUDER_LIONEL_TYPES_H
#define MCGRUDER_LIONEL_TYPES_H

/**
 * BACKLOG defineix el nombre maxim de peticions en espera
 */
#define BACKLOG 10
#define BUSTIA_JPG 0
#define JPG_TYPE 20 // Porque si
#define BUSTIA_TXT 1
#define TXT_TYPE 40 // Porque si

#include <fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <sys/msg.h>
#include <signal.h>

//      CONNECTION
//          --- TYPE ---
#define TYPE_CONNECTION 0x01
//#define TYPE_CONNECTION '1'
//          --- HEADER ---
#define HEADER_CONNECTION "[]"
#define HEADER_CONNECTION_RESPONSE_OK "[CONOK]"
#define HEADER_CONNECTION_RESPONSE_KO "[CONKO]"

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

//      DISCONNECTION
//          --- TYPE ---
#define TYPE_DISCONNECTION 0x02
//          --- HEADER ---
#define HEADER_DISCONNECTION "[]"
#define HEADER_DISCONNECTION_RESPONSE_OK "[CONOK]"
#define HEADER_DISCONNECTION_RESPONSE_KO "[CONKO]"



// Tipus propis
typedef struct{
    char* ip;
    int portMcGruders;
    int portMcTavish;
    int tempsF3;
    int fdSocket;
}Configuracio;

typedef struct{
    char* telescopeName;
    int fdMcgruder;
    pthread_t thread;
}Mcgruder;

typedef struct {
    char type;
    char* header;
    short length;
    char* data;
}Trama;

typedef struct{
    int numMcGrudersConnected;
    Mcgruder* mcgruders;
}McGrudersList;

typedef struct{
    int dia;
    int mes;
    int any;
}Data;

typedef struct{
    int hora;
    int minut;
}Hora;

typedef struct{
    Data data;
    Hora hora;
    char* name;
    int size;
}Image;

typedef struct{
    Data data;
    Hora hora;
    char* name;
    int size;
}Txt;

typedef struct{
    int numImages;
    Image* images;
}ImagesList;

typedef struct{
    int numTxt;
    Txt* txts;
}TxtList;

typedef struct{
    long type;
    int size;
}MessageJPG;

typedef struct{
    long type;
    int length;
    char filename[100];
}MessageTXT;

typedef struct{
    long type;
    pthread_t thread;
}PaquitaReader;

typedef struct{
    int numReaders;
    PaquitaReader* readers;
}PaquitaReadersList;

typedef struct{
    int numTotalImatges; // init a 0
    float totalKB; // init a 0
}PaquitaJPGinfo;

typedef struct{
    char codi[3];
    float densitat;
    float magnitud;
}Constelacio;

typedef struct{
    Constelacio* constelacions; // init a NULL
    int numConstelacions; // init a 0
    float mitjanaDensitats; // init a 1 -->  0 * 0
    float magnitudMAX; // init a 0 (pos [0] sera la 1ª a la forsa)
    float magnitudMIN; // init a 0 (pos [0] sera la 1ª a la forsa)
}LastTxtInfo;

typedef struct{
    int numTotalTxt; // init a 0
    float mitjanaConstelacionsPerArxiu; // init a 1 -->  0 * 0
}PaquitaTXTinfo;

// Variables globals
Configuracio configuracio;
struct sockaddr_in socketConfig;
McGrudersList mcGrudersList;
int pidPaquita;
int queueId;
PaquitaReadersList readersList;

ImagesList imagesList;
TxtList txtList;

PaquitaJPGinfo paquitaJPGinfo;
PaquitaTXTinfo paquitaTXTinfo;
LastTxtInfo lastTxtInfo;


#endif //MCGRUDER_LIONEL_TYPES_H
