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

    // Matem a punyetasus a paquita
    kill(pidPaquita, 9);

    // Extreiem les imatges i els txts als arxius kalkun i els alliberem la memoria
    kalkun();

    // Alliberem memoria
    alliberaConfiguracio();

    // Tanquem el socket
    close(configuracio.fdSocket);

    // Tanquem la cua de missatges amb Paquita
    msgctl(queueId, IPC_RMID, NULL);

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

        // Aturem el thread, no importa que estava fent
        pthread_cancel(mcGrudersList.mcgruders[i].thread);

        // A continuacio enviem la trama de desconnexio al mcgruder mitjancant el seu FD
        Trama disconnectionTrama;
        disconnectionTrama.type = TYPE_DISCONNECTION;
        disconnectionTrama.header = HEADER_DISCONNECTION_RESPONSE_KO;
        disconnectionTrama.length = 0;
        disconnectionTrama.data = (char*)malloc(sizeof(char) * 0);
        sendTrama(disconnectionTrama, mcGrudersList.mcgruders[i].fdMcgruder);
        free(disconnectionTrama.data);

        // Tanquem el canal de comunicacio
        close(mcGrudersList.mcgruders[i].fdMcgruder);

        // Mostrem missatge de desconnexio del mcgruder
        mcGrudersList.mcgruders[i].telescopeName[strlen(mcGrudersList.mcgruders[i].telescopeName) - 1] = '\0';
        mostraMissatgeDisconnectingMcGruder(mcGrudersList.mcgruders[i].telescopeName);

        deleteMcGruder(mcGrudersList.mcgruders[i].fdMcgruder, i);
    }

    // Desbloquejem el semafor
    pthread_mutex_unlock(&mutexLlistaMcGruders);
}

void kalkun(){
    int fdJPG = open("KalkunJPG.txt",O_WRONLY | O_CREAT | O_APPEND, 0644);
    if (fdJPG < 0){
        write(1, "No s'han pogut escriure l'arxiu KalkunJPG.txt\n", strlen("No s'han pogut escriure l'arxiu KalkunJPG.txt\n") * sizeof(char));
    }else{
        // NOTA: Sempre escriurem \n al final de cada linia

        // Esperem i bloquejem el semafor de les imatges
        pthread_mutex_lock(&mutexLlistaImatges);

        for (int i = 0; i < imagesList.numImages; i++){
            char* entrada = NULL;
            entrada = formaEntradaKalkunJPG(imagesList.images[i], entrada);
            write(fdJPG, entrada, strlen(entrada) * sizeof(char));
            free(entrada);
        }

        pthread_mutex_unlock(&mutexLlistaImatges);

        close(fdJPG);
    }

    int fdTXT = open("KalkunTXT.txt",O_WRONLY | O_CREAT | O_APPEND, 0644);
    if (fdTXT < 0){
        write(1, "No s'han pogut escriure l'arxiu KalkunTXT.txt\n", strlen("No s'han pogut escriure l'arxiu KalkunTXT.txt\n") * sizeof(char));
    }else{
        // NOTA: Sempre escriurem \n al final de cada linia

        // Esperem i bloquejem el semafor dels txt
        pthread_mutex_lock(&mutexLlistaTxts);

        for (int i = 0; i < txtList.numTxt; i++){
            char* entrada = NULL;
            entrada = formaEntradaKalkunTXT(txtList.txts[i], entrada);
            write(fdTXT, entrada, strlen(entrada) * sizeof(char));
            free(entrada);
        }

        pthread_mutex_unlock(&mutexLlistaTxts);

        close(fdTXT);
    }
}

char* formaEntradaKalkunJPG(Image image, char* entrada){
    char buffer[100];
    sprintf(buffer, "%d-%d-%d %d:%d %s %d bytes\n", image.data.any, image.data.mes, image.data.dia, image.hora.hora,
            image.hora.hora, image.name, image.size);

    entrada = strdup(buffer);
    return entrada;
}

char* formaEntradaKalkunTXT(Txt txt, char* entrada){
    char buffer[100];
    sprintf(buffer, "%d-%d-%d %d:%d %s %d bytes\n", txt.data.any, txt.data.mes, txt.data.dia, txt.hora.hora,
            txt.hora.hora, txt.name, txt.size);

    entrada = strdup(buffer);
    return entrada;
}


// Funcions de l'estructura que emmagatzema els mcgruders
void creaNouMcGruder(int fdNewMcgruder){
    int fdNewMcGruderCopy = fdNewMcgruder;

    // Rebem trama de connexio per part del mcgruder
    Trama connectionTrama = receiveTrama(fdNewMcgruder);
    int error = 0;

    if (connectionTrama.length < 0){
        mostraErrorNewMcgruder();
    }else{
        if (connectionTrama.type == TYPE_CONNECTION && strcmp(connectionTrama.header, HEADER_CONNECTION) == 0){
            // Connexio establerta
            mostraMissatgeConnectionReady(connectionTrama.data);

            int nameLength = connectionTrama.length;
            char* telescopeName = (char*)malloc(nameLength * sizeof(char) + 1);
            telescopeName = strcpy(telescopeName, connectionTrama.data);// copiem el nom

            // Enviem CONOK to mcgruder
            free(connectionTrama.header);
            connectionTrama.header = (char*)malloc(sizeof(char) * strlen(HEADER_CONNECTION_RESPONSE_OK));
            connectionTrama.header = strcpy(connectionTrama.header, HEADER_CONNECTION_RESPONSE_OK);
            connectionTrama.length = 0;
            free(connectionTrama.data);
            connectionTrama.data = (char*)malloc(sizeof(char) * 0);
            int mcgruderDown = sendTrama(connectionTrama, fdNewMcgruder);
            if (mcgruderDown){
                free(telescopeName);
                mostraErrorNewMcgruder();
            }else{
                free(connectionTrama.header);
                free(connectionTrama.data);
                // creem el nou McGruder
                Mcgruder newMcGruder;

                newMcGruder.telescopeName = (char*)malloc(sizeof(char) * nameLength + 1);
                newMcGruder.telescopeName = strcpy(newMcGruder.telescopeName, telescopeName);

                free(telescopeName);
                newMcGruder.fdMcgruder = fdNewMcgruder;
                int s = pthread_create(&newMcGruder.thread, NULL, mcGruderFunc, &fdNewMcGruderCopy);
                if (s != 0){
                    error++;
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
            connectionTrama.header = strcpy(connectionTrama.header, HEADER_CONNECTION_RESPONSE_KO);
            connectionTrama.length = 0;
            free(connectionTrama.data);
            connectionTrama.data = (char*)malloc(sizeof(char) * 0);
            sendTrama(connectionTrama, fdNewMcgruder);
            free(connectionTrama.header);
            free(connectionTrama.data);
            mostraErrorNewMcgruder();
        }
    }

}

void addNewMcGruder(Mcgruder mcgruder){
    int newSize = mcGrudersList.numMcGrudersConnected + 1;
    mcGrudersList.mcgruders = realloc(mcGrudersList.mcgruders, newSize * sizeof(Mcgruder));
    //mcGrudersList.mcgruders[newSize - 1] = mcgruder;
    mcGrudersList.mcgruders[newSize - 1].thread = mcgruder.thread;
    mcGrudersList.mcgruders[newSize - 1].telescopeName = (char*)malloc(strlen(mcgruder.telescopeName) * sizeof(char) + 1);
    mcGrudersList.mcgruders[newSize - 1].telescopeName = strcpy(mcGrudersList.mcgruders[newSize - 1].telescopeName, mcgruder.telescopeName);
    mcGrudersList.mcgruders[newSize - 1].fdMcgruder = mcgruder.fdMcgruder;
    mcGrudersList.numMcGrudersConnected = newSize;

    free(mcgruder.telescopeName);
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
                    mcGrudersList.mcgruders[i].telescopeName = strcpy(mcGrudersList.mcgruders[i].telescopeName, mcGrudersList.mcgruders[i+1].telescopeName);
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
                mcGrudersList.mcgruders[i].telescopeName = strcpy(mcGrudersList.mcgruders[i].telescopeName, mcGrudersList.mcgruders[i+1].telescopeName);
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
        mcGrudersList.mcgruders[index].telescopeName = strcpy(mcGrudersList.mcgruders[index].telescopeName, nom);
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
                    write(1, "Trama desconeguda\n", strlen("Trama desconeguda\n") * sizeof(char));
                    break;
                case 4:
                    // Extensio desconeguda (mai perque mcgruder ja controla les extensions dels arxius)
                    write(1, "Extensio de l'arxiu desconeguda\n", strlen("Extensio de l'arxiu desconeguda\n") * sizeof(char));
                    break;
                case 5:
                    // Hem rebut l'arxiu perfectament (ja no cal fer res mes)
                    break;
                case 6:
                    write(1, "Error al crear l'arxiu!\n", strlen("Error al crear l'arxiu!\n") * sizeof(char));
                    break;
                case 7:
                    write(1, "Error, no hem trobat de qui rebem l'arxiu!\n", strlen("Error, no hem trobat de qui rebem l'arxiu!\n") * sizeof(char));
                    break;
                default:
                    write(1, "Everything running OK!\n", strlen("Everything running OK!\n") * sizeof(char));
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
    mcGrudersList.mcgruders[index].telescopeName[strlen(mcGrudersList.mcgruders[index].telescopeName) - 1] = '\0';
    mostraMissatgeDisconnectingMcGruder(mcGrudersList.mcgruders[index].telescopeName);
    deleteMcGruder(fd, index);

    // Desbloquejem semafor
    pthread_mutex_unlock(&mutexLlistaMcGruders);
}