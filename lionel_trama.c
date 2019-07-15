//
// Created by xosel on 02/07/2019.
//

#include "lionel_trama.h"

extern pthread_mutex_t mutexLlistaMcGruders;
extern pthread_mutex_t mutexLlistaImatges;
extern pthread_mutex_t mutexLlistaTxts;

void mostraTrama(Trama connectionTrama){
    printf("TRAMA\n");
    printf("\tType: %#08x\n", connectionTrama.type);
    printf("\tHeader: %s\n", connectionTrama.header);
    printf("\tLength: %hu\n", connectionTrama.length);
    printf("\tData: %s\n", connectionTrama.data);
}

int sendTrama(Trama trama, int fd){
    //write(fdLionel, &trama, sizeof(trama));
    mostraTrama(trama);

    int disconnected = write(fd, &trama.type, 1);
    if (disconnected < 0){
        return 1;
    }else{
        disconnected = write(fd, trama.header, strlen(trama.header) * sizeof(char));
        if (disconnected < 0){
            return 1;
        }else{
            disconnected = write(fd, &trama.length, 2);
            if (disconnected < 0){
                return 1;
            }else{
                disconnected = write(fd, trama.data, trama.length * sizeof(char));
                if (disconnected < 0){
                    return 1;
                }
            }
        }
    }
    return 0;
}

Trama receiveTrama(int fd){
    Trama receivedTrama;
    receivedTrama.data = NULL;
    receivedTrama.header = NULL;
    memset(&receivedTrama, 0, sizeof(receivedTrama));

    int disconnected = read(fd, &receivedTrama.type, sizeof(char));
    if (disconnected <= 0){
        receivedTrama.length = -1; // Marquem que s'ha trencat la connexio
        return receivedTrama;
    }else{
        receivedTrama.header = (char*)malloc(sizeof(char));
        int i = 0;
        char c = '_';
        while (c != ']'){
            disconnected = read(fd, &c, sizeof(char));
            if (disconnected <= 0){
                receivedTrama.length = -1;
                return receivedTrama;
            } else{
                receivedTrama.header[i] = c;
                if (c != ']'){
                    i++;
                    receivedTrama.header = realloc(receivedTrama.header, sizeof(char) * (i+1));
                }
            }

        }

        disconnected = read(fd, &receivedTrama.length, 2);
        if (disconnected <= 0){
            receivedTrama.length = -1;
            return receivedTrama;
        }else{
            int nameLength = ((int)receivedTrama.length);
            receivedTrama.data = (char*)malloc(sizeof(char) * nameLength);
            for (i = 0; i < nameLength; i++) {
                disconnected = read(fd, &c, sizeof(char));
                if (disconnected <= 0){
                    receivedTrama.length = -1;
                    return receivedTrama;
                }else{
                    receivedTrama.data[i] = c;
                }

            }

            mostraTrama(receivedTrama);
            return receivedTrama;
        }

    }
}

int tractaTrama(Trama received, int fd){
    switch (received.type){
        case TYPE_CONNECTION:
            if (strcmp(received.header, HEADER_CONNECTION) == 0){
                free(received.header);
                received.header = (char*)malloc(sizeof(char) * strlen(HEADER_CONNECTION_RESPONSE_OK));
                strcpy(received.header, HEADER_CONNECTION_RESPONSE_OK);

                received.length = 0;
                free(received.data);
                received.data = (char*)malloc(sizeof(char) * 0);
                int mcGruderDown = sendTrama(received, fd);
                if (mcGruderDown){
                    return 1;
                }
                return 0;
            }else{
                // Trama desconeguda --> no fem res :)
                return 3;
            }

        case TYPE_SENDFILE:
            if (strcmp(received.header, HEADER_SENDFILE_METADATA) == 0){
                int tipus = extensioMetadata(received.data);
                Image image;
                Txt txt;
                char* mcgrduerName;
                if (tipus != 0){
                    switch (tipus){
                        case 1:
                            // Imatge JPG
                            image = getImageInfo(received);

                            // Mostrem missatge rebent informacio del mcgruder
                            mcgrduerName = getMcGruderName(fd);
                            if (mcgrduerName == NULL){
                                mostraErrorRebreArxiu(image.name);
                                return 7;
                            }
                            mostraMissatgeReceivingData(mcgrduerName);

                            char* fileContent = (char*)malloc(image.size * sizeof(char));
                            while (1){
                                received = receiveTrama(fd);
                                if (received.length < 0){
                                    // McGruder ha caigut
                                    mostraErrorRebreArxiu(image.name);
                                    free(fileContent);
                                    return 1; // McGruder desconnectat
                                }else{
                                    if (received.type == TYPE_DISCONNECTION){
                                        free(fileContent);
                                        mostraErrorRebreArxiu(image.name);
                                        mostraMissatgeDisconnectingMcGruder(received.data);
                                        if (strcmp(received.header, HEADER_DISCONNECTION) == 0){
                                            free(received.header);
                                            received.header = (char*)malloc(sizeof(char) * strlen(HEADER_DISCONNECTION_RESPONSE_OK));
                                            strcpy(received.header, HEADER_DISCONNECTION_RESPONSE_OK);

                                            received.length = 0;
                                            free(received.data);
                                            received.data = (char*)malloc(sizeof(char) * 0);
                                            int mcGruderDown = sendTrama(received, fd);
                                            if (mcGruderDown){
                                                return 1;
                                            }else{
                                                return 2;
                                            }
                                        }else{
                                            // Trama desconeguda --> no fem res :)
                                            printf("Trama desconeguda per part del fd = %d\n", fd);
                                            return 3;
                                        }
                                    }else{
                                        if (strcmp(received.header, HEADER_SENDFILE_DATA) == 0){
                                            fileContent = strcat(fileContent, received.data);
                                        } else{
                                            break;
                                        }
                                    }
                                }
                            }

                            // Ja hem acabat de llegir tot el contingut i hem rebut la trama de endfile
                            if (received.length < 0){
                                free(fileContent);
                                mostraErrorRebreArxiu(image.name);
                                return 1;
                            }

                            // Hem rebut be la imatge, muntem la imatge
                            char* path = (char*)malloc(0 * sizeof(char));
                            path = strcpy(path, "files/");
                            path = strcat(path, image.name);

                            int fdImatge = creat(path, 0777);
                            if (fdImatge < 0){
                                printf("Error al intentar crear arxiu!\n");
                                free(fileContent);
                                return 6;
                            }
                            write(fdImatge, fileContent, image.size * sizeof(char));
                            close(fdImatge);

                            // comprovem el checksum
                            char* myChecksum = makeChecksum(path);

                            // Comparem el checksum calculat (provisional) amb el rebut
                            if (strcmp(myChecksum, received.data) == 0){
                                // Ambdos checksum coincideixen
                                printf("Hem rebut be l'arxiu! \n");

                                Trama tramaChecksumOk;
                                tramaChecksumOk.type = TYPE_SENDFILE;
                                int length = strlen(HEADER_SENDFILE_RESPONSE_OK_IMAGE);
                                tramaChecksumOk.header = (char*)malloc(length * sizeof(char));
                                tramaChecksumOk.header = strcpy(tramaChecksumOk.header, HEADER_SENDFILE_RESPONSE_OK_IMAGE);

                                tramaChecksumOk.length = 0;
                                tramaChecksumOk.data = (char*)malloc(sizeof(char) * 0);
                                //mostraTrama(tramaChecksumOk);
                                int disconnected = sendTrama(tramaChecksumOk, fd);

                                // Si mcgruder es desconnecta abans de rebre la trama de OK del checksum eliminem l'arxiu
                                if (disconnected){
                                    remove(path); // El borrem perque mcgruder no el borrara i ens el tornara a enviar quan es torni a connectar
                                    return 1; // Mc gruder desconnectat
                                }

                                addNewImage(image);
                                mostraMissatgeFileReceived(image.name);

                                return 5;
                            }else{
                                // Checksum error
                                mostraErrorRebreArxiu(image.name);

                                Trama tramaChecksumOk;
                                tramaChecksumOk.type = TYPE_SENDFILE;
                                int length = strlen(HEADER_SENDFILE_RESPONSE_KO_IMAGE);
                                tramaChecksumOk.header = (char*)malloc(length * sizeof(char));
                                tramaChecksumOk.header = strcpy(tramaChecksumOk.header, HEADER_SENDFILE_RESPONSE_KO_IMAGE);

                                tramaChecksumOk.length = 0;
                                tramaChecksumOk.data = (char*)malloc(sizeof(char) * 0);
                                //mostraTrama(tramaChecksumOk);
                                int disconnected = sendTrama(tramaChecksumOk, fd);
                                return 6;
                            }

                            break;
                        case 2:
                            // Text TXT
                            printf("Es un fitxer de text!\n");
                            break;
                    }
                    return 0;
                }else{
                    return 4; // extensio desconeguda
                }
            }else {
                return 3; // trama desconeguda
            }
            return 0;
        case TYPE_DISCONNECTION:
            mostraMissatgeDisconnectingMcGruder(received.data);
            if (strcmp(received.header, HEADER_DISCONNECTION) == 0){
                free(received.header);
                received.header = (char*)malloc(sizeof(char) * strlen(HEADER_DISCONNECTION_RESPONSE_OK));
                strcpy(received.header, HEADER_DISCONNECTION_RESPONSE_OK);

                received.length = 0;
                free(received.data);
                received.data = (char*)malloc(sizeof(char) * 0);
                int mcGruderDown = sendTrama(received, fd);
                if (mcGruderDown){
                    return 1;
                }else{
                    return 2;
                }
            }else{
                // Trama desconeguda --> no fem res :)
                printf("Trama desconeguda per part del fd = %d\n", fd);
                return 3;
            }
    }
    return 0;
}

// Funcions per a tractar els camps de les trames
int extensioMetadata(char* receivedData){
    char extensio[4];
    for (int i = 0; i < 4; i++) {
        extensio[i] = receivedData[i + 1];
    }
    if (strcmp(extensio, ".jpg") == 0){
        return 1;
    }else if (strcmp(extensio, ".txt") == 0){
        return 2;
    }else{
        return 0;
    }
}

Image getImageInfo(Trama received){
    // creem la imatge
    Image image;

    image.size = 0;
    // Obtenim el tamany de la imatge
    int length = received.length;
    int i = 6;
    while (received.data[i] != '&'){
        image.size = image.size * 10 + (received.data[i] - '0');
        i++;
    }

    i++;

    int any = 0;
    // Obtenim l'any
    while (received.data[i] != '-'){
        any = any * 10 + (received.data[i] - '0');
        i++;
    }

    i++;

    int mes = 0;
    // Obtenim el mes
    while (received.data[i] != '-'){
        mes = mes * 10 + (received.data[i] - '0');
        i++;
    }

    i++;

    int dia = 0;
    // Obtenim el dia
    while (received.data[i] != ' '){
        dia = dia * 10 + (received.data[i] - '0');
        i++;
    }

    image.data.any = any;
    image.data.mes = mes;
    image.data.dia = dia;

    i++;

    int hora = 0;
    // Obtenim la hora
    while (received.data[i] != ':'){
        hora = hora * 10 + (received.data[i] - '0');
        i++;
    }

    i++;

    int minut = 0;
    // Obtenim el minut
    while (received.data[i] != '&'){
        minut = minut * 10 + (received.data[i] - '0');
        i++;
    }

    image.hora.hora = hora;
    image.hora.minut = minut;

    i++;

    // Obtenim el nom del fitxer
    int nameLength = received.length - i;
    image.name = (char*)malloc(nameLength * sizeof(char));
    for (int k = 0; k < nameLength - 1; k++) {
        image.name[k] = received.data[i+k];
    }

    return image;
}

void addNewImage(Image newImage){

    // Esperem al semafor i el bloquejem
    pthread_mutex_lock(&mutexLlistaImatges);

    int index = imagesList.numImages;
    imagesList.numImages++;
    imagesList.images = realloc(imagesList.images, imagesList.numImages * sizeof(Image));

    // Copiem els valors de la nova imatge
    imagesList.images[index].name = (char*)malloc(sizeof(char) * strlen(newImage.name));
    imagesList.images[index].name = strcpy(imagesList.images[index].name, newImage.name);
    imagesList.images[index].size = newImage.size;
    imagesList.images[index].data = newImage.data;
    imagesList.images[index].hora = newImage.hora;

    printf("Nova imatge desada!\n");
    int numImatges = imagesList.numImages;
    printf("Tenim %d imatges guardades \n", numImatges);
    for (int i = 0; i < numImatges; i++) {
        printf("Imatge: %s \n", imagesList.images[i].name);
    }

    // Desbloquejem el semafor
    pthread_mutex_unlock(&mutexLlistaImatges);

}

char* getMcGruderName(int fd){
    // Esperem al semafor dels mcgruders
    pthread_mutex_lock(&mutexLlistaMcGruders);
    int index = -1;

    for(int i = 0; i < mcGrudersList.numMcGrudersConnected; i++){
        if (mcGrudersList.mcgruders[i].fdMcgruder == fd){
            index = i;
            break;
        }
    }

    char* mcgruderName;

    if (index == -1){
        mcgruderName = NULL;
    }else{
        mcgruderName = (char*)malloc(sizeof(char) * strlen(mcGrudersList.mcgruders[index].telescopeName));
        mcgruderName = strcpy(mcgruderName, mcGrudersList.mcgruders[index].telescopeName);
    }

    // Desbloquejem el semafor
    pthread_mutex_unlock(&mutexLlistaMcGruders);

    return mcgruderName;
}