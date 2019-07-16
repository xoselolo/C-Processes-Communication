//
// Created by xosel on 02/07/2019.
//

#include "mcgruder_trama.h"

void mostraTrama(Trama connectionTrama){
    printf("TRAMA\n");
    printf("\tType: %#08x\n", connectionTrama.type);
    printf("\tHeader: %s\n", connectionTrama.header);
    printf("\tLength: %hu\n", connectionTrama.length);
    printf("\tData: %s\n", connectionTrama.data);
}

int sendTrama(Trama trama){
    //mostraTrama(trama);
    int desconnexio = write(fdLionel, &trama.type, 1);
    if (desconnexio < 0){
        return 1;
        // Ja ens podem desconnectar, perque Lionel ha caigut
    }else{
        desconnexio = write(fdLionel, trama.header, strlen(trama.header) * sizeof(char));
        if (desconnexio < 0){
            return 1;
            // Ja ens podem desconnectar, perque Lionel ha caigut
        }else{
            desconnexio = write(fdLionel, &trama.length, 2);
            if (desconnexio < 0){
                return 1;
                // Ja ens podem desconnectar, perque Lionel ha caigut
            }else{
                desconnexio = write(fdLionel, trama.data, trama.length * sizeof(char));
                if (desconnexio < 0){
                    return 1;
                    // Ja ens podem desconnectar, perque Lionel ha caigut
                }
            }
        }
    }

    return 0;
}

int enviaTramaDesconnexio(){
    // Enviem trama de desconnexio
    Trama disconnectionTrama;
    disconnectionTrama.type = TYPE_DISCONNECTION;
    disconnectionTrama.header = HEADER_DISCONNECTION;
    disconnectionTrama.length = (short) strlen(configuracio.telescopeName);
    disconnectionTrama.data = (char*) malloc(disconnectionTrama.length * sizeof(char));
    disconnectionTrama.data = strcpy(disconnectionTrama.data, configuracio.telescopeName);
    int lionelDown = sendTrama(disconnectionTrama);
    free(disconnectionTrama.data);
    if (lionelDown){
        // Lionel ha caigut
        return 1;
    }
    return 0;
}

/*Trama receiveTrama(){
    Trama receivedTrama;

    memset(&receivedTrama, 0, sizeof(receivedTrama));

    int disconnected = read(fdLionel, &receivedTrama.type, sizeof(char));
    if (disconnected <= 0){
        receivedTrama.length = -1;
        return receivedTrama;
    }
    receivedTrama.header = (char*)malloc(sizeof(char));
    int i = 0;
    char c = '_';
    while (c != ']'){
        disconnected = read(fdLionel, &c, sizeof(char));
        if (disconnected <= 0){
            receivedTrama.length = -1;
            return receivedTrama;
        }
        receivedTrama.header[i] = c;
        if (c != ']'){
            i++;
            receivedTrama.header = realloc(receivedTrama.header, sizeof(char) * (i+1));
        }
    }

    disconnected = read(fdLionel, &receivedTrama.length, 2);
    if (disconnected <= 0){
        receivedTrama.length = -1;
        return receivedTrama;
    }
    int nameLength = ((int)receivedTrama.length);
    receivedTrama.data = (char*)malloc(sizeof(char) * nameLength);
    for (i = 0; i < nameLength; i++) {
        disconnected = read(fdLionel, &c, sizeof(char));
        if (disconnected <= 0){
            receivedTrama.length = -1;
            return receivedTrama;
        }
        receivedTrama.data[i] = c;
    }

    mostraTrama(receivedTrama);
    return receivedTrama;
}*/

Trama receiveTrama(){
    Trama receivedTrama;
    receivedTrama.header = NULL;
    receivedTrama.data = NULL;
    memset(&receivedTrama, 0, sizeof(receivedTrama));

    int disconnected = read(fdLionel, &receivedTrama.type, sizeof(char));
    if (disconnected <= 0){
        receivedTrama.length = -1; // Marquem que s'ha trencat la connexio
        return receivedTrama;
    }else{
        receivedTrama.header = (char*)malloc(sizeof(char));
        int i = 0;
        char c = '_';
        while (c != ']'){
            disconnected = read(fdLionel, &c, sizeof(char));
            if (disconnected <= 0){
                receivedTrama.length = -1;
                return receivedTrama;
            } else{
                receivedTrama.header[i] = c;

                i++;
                receivedTrama.header = realloc(receivedTrama.header, sizeof(char) * (i+1));

            }
        }
        receivedTrama.header[i] = '\0';

        disconnected = read(fdLionel, &receivedTrama.length, 2);
        if (disconnected <= 0){
            receivedTrama.length = -1;
            return receivedTrama;
        }else{
            int nameLength = ((int)receivedTrama.length);
            receivedTrama.data = (char*)malloc(sizeof(char) * (nameLength + 1));
            for (i = 0; i < nameLength; i++) {
                disconnected = read(fdLionel, &c, sizeof(char));
                if (disconnected <= 0){
                    receivedTrama.length = -1;
                    return receivedTrama;
                }else{
                    receivedTrama.data[i] = c;
                }
            }
            receivedTrama.data[nameLength] = '\0';

            //mostraTrama(receivedTrama);
            return receivedTrama;
        }

    }
}

int sendConnectionTrama(){
    Trama connectionTrama;

    connectionTrama.type = TYPE_CONNECTION;
    connectionTrama.header = (char*)malloc(strlen(HEADER_CONNECTION) * sizeof(char));
    connectionTrama.header = strcpy(connectionTrama.header, HEADER_CONNECTION);
    connectionTrama.length = (short) strlen(configuracio.telescopeName);
    connectionTrama.data = (char*)malloc(sizeof(connectionTrama.length));
    connectionTrama.data = strcpy(connectionTrama.data, configuracio.telescopeName);

    int connError = sendTrama(connectionTrama);
    free(connectionTrama.header);
    free(connectionTrama.data);

    if (connError){
        mostraErrorConnexio();
        return 1;
    }

    connectionTrama = receiveTrama();
    if (connectionTrama.length < 0){
        mostraErrorConnexio();
        free(connectionTrama.header);
        free(connectionTrama.data);
        return 1;
    }

    if (strcmp(connectionTrama.header, HEADER_CONNECTION_RESPONSE_KO) == 0){
        mostraErrorConnexio();
        free(connectionTrama.data);
        free(connectionTrama.header);
        return 1;
    }else{
        mostraMissatgeConnectionReady();
        free(connectionTrama.data);
        free(connectionTrama.header);
        return 0;
    }
}