//
// Created by xosel on 27/06/2019.
//

#include "lionel_connection.h"

extern pthread_mutex_t mutexLlistaMcGruders;
extern pthread_mutex_t mutexLlistaImatges;
extern pthread_mutex_t mutexLlistaTxts;

void desconnecta(){
    // Matem els mcgruders (dedicats) notificant als clients (telescopis) que estem desconnectant-nos
    killMcGruders();
    // Destruim el semafor dels mcgruders
    pthread_mutex_destroy(&mutexLlistaMcGruders);

    // Alliberem memoria
    alliberaConfiguracio();

    // Tanquem el socket
    close(configuracio.fdSocket);

    // Mostrem el missatge de que ens desconnectem
    mostraMissatgeDisconnecting();

    // Finalitzem l'execucio
    exit(EXIT_SUCCESS);
}

void killMcGruders(){
    // Esperem a obtenir la memoria bloquejada pel semafor i la bloquejem
    pthread_mutex_lock(&mutexLlistaMcGruders);

    // Matem tots els threads
    for (int i = mcGrudersList.numMcGrudersConnected - 1; i >= 0; i--) {
        printf("\nMatant al thread %d \n", i);

        // Aturem el thread, no importa que estava fent
        pthread_cancel(mcGrudersList.mcgruders[i].thread);

        // A continuacio enviem la trama de desconnexio al mcgruder mitjancant el seu FD
        Trama disconnectionTrama;
        disconnectionTrama.type = TYPE_DISCONNECTION;
        disconnectionTrama.header = HEADER_DISCONNECTION_RESPONSE_KO;
        disconnectionTrama.length = 0;
        disconnectionTrama.data = (char*)malloc(sizeof(char) * 0);
        sendTrama(disconnectionTrama, mcGrudersList.mcgruders[i].fdMcgruder);

        // Tanquem el canal de comunicacio
        close(mcGrudersList.mcgruders[i].fdMcgruder);

        // Mostrem missatge de desconnexio del mcgruder
        mostraMissatgeDisconnectingMcGruder(mcGrudersList.mcgruders[i].telescopeName);

        deleteMcGruder(mcGrudersList.mcgruders[i].fdMcgruder, i);
    }

    // Desbloquejem el semafor
    pthread_mutex_unlock(&mutexLlistaMcGruders);
}


// Funcions de l'estructura que emmagatzema els mcgruders
void creaNouMcGruder(int fdNewMcgruder){
    int fdNewMcGruderCopy = fdNewMcgruder;

    // Rebem trama de connexio per part del mcgruder
    Trama connectionTrama = receiveTrama(fdNewMcgruder);
    int error = 0;

    if (connectionTrama.length < 0){
        printf("ERRRR 1\n");
        mostraErrorNewMcgruder();
    }else{
        if (connectionTrama.type == TYPE_CONNECTION && strcmp(connectionTrama.header, HEADER_CONNECTION) == 0){
            // Connexio establerta
            mostraMissatgeConnectionReady(connectionTrama.data);

            int nameLength = connectionTrama.length;
            char* telescopeName = (char*)malloc(nameLength * sizeof(char));
            telescopeName = strcpy(telescopeName, connectionTrama.data);// copiem el nom

            // Enviem CONOK to mcgruder
            free(connectionTrama.header);
            connectionTrama.header = (char*)malloc(sizeof(char) * strlen(HEADER_CONNECTION_RESPONSE_OK));
            strcpy(connectionTrama.header, HEADER_CONNECTION_RESPONSE_OK);
            connectionTrama.length = 0;
            free(connectionTrama.data);
            connectionTrama.data = (char*)malloc(sizeof(char) * 0);
            int mcgruderDown = sendTrama(connectionTrama, fdNewMcgruder);
            if (mcgruderDown){
                printf("ERRRR 2\n");
                mostraErrorNewMcgruder();
            }else{
                // creem el nou McGruder
                Mcgruder newMcGruder;

                newMcGruder.telescopeName = (char*)malloc(sizeof(char) * nameLength);
                strcpy(newMcGruder.telescopeName, telescopeName);

                free(telescopeName);
                newMcGruder.fdMcgruder = fdNewMcgruder;
                int s = pthread_create(&newMcGruder.thread, NULL, mcGruderFunc, &fdNewMcGruderCopy);
                if (s != 0){
                    error++;
                    printf("ERRRR 3\n");
                }else{
                    // Thread creat correctament, l'afegim a la llista de mcgruders
                    pthread_mutex_lock(&mutexLlistaMcGruders); // Esperem semafor i bloquejem
                    addNewMcGruder(newMcGruder);
                    pthread_mutex_unlock(&mutexLlistaMcGruders); // Desbloquejem semafor
                }
            }

        }else{
            error++;
        }

        if (error > 0){
            // Enviem CONKO to mcgruder
            free(connectionTrama.header);
            connectionTrama.header = (char*)malloc(sizeof(char) * strlen(HEADER_CONNECTION_RESPONSE_KO));
            strcpy(connectionTrama.header, HEADER_CONNECTION_RESPONSE_KO);
            connectionTrama.length = 0;
            free(connectionTrama.data);
            connectionTrama.data = (char*)malloc(sizeof(char) * 0);
            sendTrama(connectionTrama, fdNewMcgruder);
            printf("ERRRR 4\n");
            mostraErrorNewMcgruder();
        }
    }

}

void addNewMcGruder(Mcgruder mcgruder){
    int newSize = mcGrudersList.numMcGrudersConnected + 1;
    mcGrudersList.mcgruders = realloc(mcGrudersList.mcgruders, newSize * sizeof(Mcgruder));
    //mcGrudersList.mcgruders[newSize - 1] = mcgruder;
    mcGrudersList.mcgruders[newSize - 1].thread = mcgruder.thread;
    mcGrudersList.mcgruders[newSize - 1].telescopeName = (char*)malloc(strlen(mcgruder.telescopeName) * sizeof(char));
    strcpy(mcGrudersList.mcgruders[newSize - 1].telescopeName, mcgruder.telescopeName);
    mcGrudersList.mcgruders[newSize - 1].fdMcgruder = mcgruder.fdMcgruder;
    mcGrudersList.numMcGrudersConnected = newSize;
}

int indexOfMcGruder(int fdMcGruder){
    if (mcGrudersList.numMcGrudersConnected <= 0){
        return -1; // No McGruders
    }
    else{
        for (int i = 0; i < mcGrudersList.numMcGrudersConnected; i++) {
            if (mcGrudersList.mcgruders[i].fdMcgruder == fdMcGruder){
                return i;
            }
        }
        return -2; // Not found
    }
}

int deleteMcGruder(int fdMcGruder, int indexKnown){
    if (indexKnown == -1){
        int index = indexOfMcGruder(fdMcGruder);
        if (index >= 0){
            for (int i = index; i < mcGrudersList.numMcGrudersConnected; i++) {
                if (i + 1 < mcGrudersList.numMcGrudersConnected){
                    // copiem element [i+1] a [i]
                    mcGrudersList.mcgruders[i].fdMcgruder = mcGrudersList.mcgruders[i+1].fdMcgruder;
                    strcpy(mcGrudersList.mcgruders[i].telescopeName, mcGrudersList.mcgruders[i+1].telescopeName);
                }
            }

            free(mcGrudersList.mcgruders[mcGrudersList.numMcGrudersConnected - 1].telescopeName);

            if (mcGrudersList.numMcGrudersConnected > 1){
                mcGrudersList.mcgruders = realloc(mcGrudersList.mcgruders, (mcGrudersList.numMcGrudersConnected-1) * sizeof(Mcgruder));
            }else{
                free(mcGrudersList.mcgruders);
                mcGrudersList.mcgruders = NULL;
            }
            mcGrudersList.numMcGrudersConnected = mcGrudersList.numMcGrudersConnected - 1;

            return 1;
        }else{
            return -1;
        }
    }else{
        for (int i = indexKnown; i < mcGrudersList.numMcGrudersConnected; i++) {
            if (i + 1 < mcGrudersList.numMcGrudersConnected){
                // copy i+1 into i
                mcGrudersList.mcgruders[i].fdMcgruder = mcGrudersList.mcgruders[i+1].fdMcgruder;
                strcpy(mcGrudersList.mcgruders[i].telescopeName, mcGrudersList.mcgruders[i+1].telescopeName);
            }
        }

        free(mcGrudersList.mcgruders[mcGrudersList.numMcGrudersConnected - 1].telescopeName);

        if (mcGrudersList.numMcGrudersConnected > 1){
            mcGrudersList.mcgruders = realloc(mcGrudersList.mcgruders, (mcGrudersList.numMcGrudersConnected-1) * sizeof(Mcgruder));
        }else{
            free(mcGrudersList.mcgruders);
            mcGrudersList.mcgruders = NULL;
        }
        mcGrudersList.numMcGrudersConnected = mcGrudersList.numMcGrudersConnected - 1;
        return 1;
    }

}


void posaNomMcGruder(char* nom, int fdMcgruder){
    int index = indexOfMcGruder(fdMcgruder);
    if (index > 0){
        strcpy(mcGrudersList.mcgruders[index].telescopeName, nom);
    }
}



// Funcio del servidor dedicat de McGruder
void * mcGruderFunc(void* arg){
    int fd = *((int*)arg);

    while (1){
        Trama received = receiveTrama(fd);
        if (received.length < 0){
            // S'ha tancat la connexio sobtadament (McGruder down)
            mcGruderDisconnectedElimination(fd);
            break;
        }else{
            //mostraTrama(received);

            int mcGruderDown = tractaTrama(received, fd);
            switch (mcGruderDown){
                case 1:
                    // McGruder amb FD ha caigut, l'hem d'eliminar
                    mcGruderDisconnectedElimination(fd);
                    return NULL;
                    break;
                case 2:
                    // Hem rebut trama de desconnexio i hem pogut enviar trama de desconnexio
                    pthread_mutex_lock(&mutexLlistaMcGruders);
                    deleteMcGruder(fd, -1);
                    pthread_mutex_unlock(&mutexLlistaMcGruders);
                    mostraMissatgeWaiting();
                    close(fd);
                    return NULL;

                case 3:
                    // Hem rebut una trama desconeguda
                    printf("Trama desconeguda\n");
                    break;
                case 4:
                    // Extensio desconeguda (mai perque mcgruder ja controla les extensions dels arxius)
                    printf("Extensio de l'arxiu desconeguda\n");
                    break;
                case 5:
                    printf("Hem rebut be l'arxiu! \n");

                    // todo : Calcular checksum

                    Trama tramaChecksumOk;
                    tramaChecksumOk.type = TYPE_SENDFILE;
                    int length = strlen(HEADER_SENDFILE_RESPONSE_OK_IMAGE);
                    printf("EL length que falla es %d \n", length);
                    tramaChecksumOk.header = (char*)malloc(length * sizeof(char));
                    tramaChecksumOk.header = strcpy(tramaChecksumOk.header, HEADER_SENDFILE_RESPONSE_OK_IMAGE);

                    //tramaChecksumOk.header = HEADER_SENDFILE_RESPONSE_OK_IMAGE;
                    tramaChecksumOk.length = 0;
                    tramaChecksumOk.data = (char*)malloc(sizeof(char) * 0);
                    //mostraTrama(tramaChecksumOk);
                    int disconnected = sendTrama(tramaChecksumOk, fd);

                    // Si mcgruder es desconnecta abans de rebre la trama de OK del checksum eliminem l'arxiu
                    if (disconnected){
                        // todo: delete file
                        mcGruderDisconnectedElimination(fd);
                        return NULL;
                    }



                    break;
                case 6:
                    break;
                default:
                    printf("Everything running OK!\n");
                    break;
            }
        }
        mostraMissatgeWaiting();

    }
    return NULL;
}

void mcGruderDisconnectedElimination(int fd){
    // Bloquejem semafor
    pthread_mutex_lock(&mutexLlistaMcGruders);

    int index = indexOfMcGruder(fd);
    mostraMissatgeDisconnectingMcGruder(mcGrudersList.mcgruders[index].telescopeName);
    deleteMcGruder(fd, index);

    // Desbloquejem semafor
    pthread_mutex_unlock(&mutexLlistaMcGruders);
}