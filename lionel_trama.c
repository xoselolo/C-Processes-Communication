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
    //mostraTrama(trama);

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
    receivedTrama.header = NULL;
    receivedTrama.data = NULL;
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

                i++;
                receivedTrama.header = realloc(receivedTrama.header, sizeof(char) * (i+1));

            }
        }
        receivedTrama.header[i] = '\0';

        disconnected = read(fd, &receivedTrama.length, 2);
        if (disconnected <= 0){
            receivedTrama.length = -1;
            return receivedTrama;
        }else{
            int nameLength = ((int)receivedTrama.length);
            receivedTrama.data = (char*)malloc(sizeof(char) * (nameLength + 1));
            for (i = 0; i < nameLength; i++) {
                disconnected = read(fd, &c, sizeof(char));
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

int tractaTrama(Trama received, int fd){
    switch (received.type){
        case TYPE_CONNECTION:
            if (strcmp(received.header, HEADER_CONNECTION) == 0){
                free(received.header);
                received.header = (char*)malloc(sizeof(char) * strlen(HEADER_CONNECTION_RESPONSE_OK));
                received.header = strcpy(received.header, HEADER_CONNECTION_RESPONSE_OK);

                received.length = 0;
                free(received.data);
                received.data = (char*)malloc(sizeof(char) * 0);

                int mcGruderDown = sendTrama(received, fd);
                free(received.header);
                free(received.data);

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
                char* mcgruderName;
                if (tipus != 0){
                    switch (tipus){
                        case 1:
                            // Imatge JPG
                            image = getImageInfo(received);

                            // Mostrem missatge rebent informacio del mcgruder
                            mcgruderName = getMcGruderName(fd);
                            if (mcgruderName == NULL){
                                mostraErrorRebreArxiu(image.name);
                                return 7;
                            }

                            mostraMissatgeReceivingData(mcgruderName);

                            int length = strlen(image.name) + 6;
                            char* path = strdup("files/\0");
                            path = (char*) realloc(path, (length + 1) * sizeof(char));
                            path = strcat(path, image.name);
                            path[length] = '\0';

                            int fdImatge = creat(path, 0777);
                            if (fdImatge < 0){
                                printf("Error al intentar crear arxiu!\n");
                                free(path);
                                return 6;
                            }

                            //char* fileContent = (char*)malloc(image.size * sizeof(char));
                            while (1){
                                received = receiveTrama(fd);
                                if (received.length < 0){
                                    // McGruder ha caigut
                                    mostraErrorRebreArxiu(image.name);
                                    remove(path);
                                    //free(fileContent);
                                    return 1; // McGruder desconnectat
                                }else{
                                    if (received.type == TYPE_DISCONNECTION){
                                        remove(path);
                                        //free(fileContent);
                                        mostraErrorRebreArxiu(image.name);
                                        received.data[strlen(received.data) - 1] = '\0';
                                        mostraMissatgeDisconnectingMcGruder(received.data);
                                        if (strcmp(received.header, HEADER_DISCONNECTION) == 0){
                                            free(received.header);
                                            received.header = (char*)malloc(sizeof(char) * strlen(HEADER_DISCONNECTION_RESPONSE_OK));
                                            received.header = strcpy(received.header, HEADER_DISCONNECTION_RESPONSE_OK);

                                            received.length = 0;
                                            free(received.data);
                                            received.data = (char*)malloc(sizeof(char) * 0);

                                            int mcGruderDown = sendTrama(received, fd);
                                            printf("HOLA1\n");
                                            free(received.header);
                                            free(received.data);
                                            printf("HOLA2\n");

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
                                            write(fdImatge, received.data, received.length * sizeof(char));
                                        } else{
                                            break;
                                        }
                                    }
                                }
                            }

                            close(fdImatge);

                            // comprovem el checksum
                            char* myChecksum = makeChecksum(path);
                            if (myChecksum == NULL){
                                printf("NO FORK\n");
                                mostraErrorRebreArxiu(image.name);
                                return 6;
                            }
                            printf("Lionel checksum -%s- \n", myChecksum);

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
                                int disconnected = sendTrama(tramaChecksumOk, fd);
                                free(tramaChecksumOk.header);
                                free(tramaChecksumOk.data);

                                // Si mcgruder es desconnecta abans de rebre la trama de OK del checksum eliminem l'arxiu
                                if (disconnected){
                                    remove(path); // El borrem perque mcgruder no el borrara i ens el tornara a enviar quan es torni a connectar
                                    free(path);
                                    free(myChecksum);
                                    return 1; // Mc gruder desconnectat
                                }

                                mostraMissatgeFileReceived(image.name);

                                addNewImage(image);

                                free(path);
                                free(myChecksum);
                                return 5;
                            }else{
                                // Checksum error
                                mostraErrorRebreArxiu(image.name);

                                Trama tramaChecksumKo;
                                tramaChecksumKo.type = TYPE_SENDFILE;
                                int length = strlen(HEADER_SENDFILE_RESPONSE_KO_IMAGE);
                                tramaChecksumKo.header = (char*)malloc(length * sizeof(char));
                                tramaChecksumKo.header = strcpy(tramaChecksumKo.header, HEADER_SENDFILE_RESPONSE_KO_IMAGE);

                                tramaChecksumKo.length = 0;
                                tramaChecksumKo.data = (char*)malloc(sizeof(char) * 0);

                                //remove(path);// Eliminem la imatge perque no la hem rebut be
                                free(path);

                                int disconnected = sendTrama(tramaChecksumKo, fd);
                                free(tramaChecksumKo.header);
                                free(tramaChecksumKo.data);

                                if (disconnected){
                                    return 1;
                                }
                                return 6;
                            }

                            break;
                        case 2:
                            // Text TXT --------------------------------------------------------------------------------
                            printf("Es un fitxer de text!\n");
                            txt = getTextInfo(received);

                            // Mostrem missatge rebent informacio del mcgruder
                            mcgruderName = getMcGruderName(fd);
                            if (mcgruderName == NULL){
                                mostraErrorRebreArxiu(txt.name);
                                return 7;
                            }
                            mostraMissatgeReceivingData(mcgruderName);

                            int lengthTxt = strlen(txt.name) + 6;
                            char* pathTxt = strdup("files/\0");
                            pathTxt = (char*) realloc(pathTxt, (lengthTxt + 1) * sizeof(char));
                            pathTxt = strcat(pathTxt, txt.name);
                            pathTxt[lengthTxt] = '\0';

                            int fdTxt = creat(pathTxt, 0777);
                            if (fdTxt < 0){
                                printf("Error al intentar crear arxiu!\n");
                                free(pathTxt);
                                return 6;
                            }

                            // Hem creat l'arxiu buit
                            // Ara rebem la trama del contingut del fitxer
                            Trama txtContentTrama = receiveTrama(fd);
                            if (txtContentTrama.length < 0){
                                remove(pathTxt);
                                mostraErrorRebreArxiu(txt.name);
                                return 1;
                            }

                            // Escribim el camp data a fuego al arxiu
                            write(fdTxt, txtContentTrama.data, txt.size * sizeof(char));

                            close(fdTxt);

                            Trama fileOkTrama;
                            fileOkTrama.type = TYPE_SENDFILE;
                            fileOkTrama.header = HEADER_SENDFILE_RESPONSE_OK_TEXT;
                            fileOkTrama.length = 0;
                            fileOkTrama.data = (char*)malloc(sizeof(char));

                            int disconnected = sendTrama(fileOkTrama, fd);
                            if (disconnected){
                                remove(pathTxt);
                                mostraErrorRebreArxiu(txt.name);
                                return 1;
                            }

                            addNewTxt(txt);
                            mostraMissatgeFileReceived(txt.name);

                            free(pathTxt);
                            return 5;
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
            received.data[strlen(received.data) - 1] = '\0';
            mostraMissatgeDisconnectingMcGruder(received.data);
            if (strcmp(received.header, HEADER_DISCONNECTION) == 0){
                free(received.header);
                received.header = (char*)malloc(sizeof(char) * strlen(HEADER_DISCONNECTION_RESPONSE_OK));
                received.header = strcpy(received.header, HEADER_DISCONNECTION_RESPONSE_OK);

                received.length = 0;
                free(received.data);
                received.data = (char*)malloc(sizeof(char) * 0);
                int mcGruderDown = sendTrama(received, fd);
                free(received.header);
                free(received.data);

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
    image.name = (char*)malloc((nameLength + 1) * sizeof(char));
    for (int k = 0; k < nameLength - 1; k++) {
        image.name[k] = received.data[i+k];
    }
    image.name[nameLength - 1] = '\0';

    return image;
}

Txt getTextInfo(Trama received){
    // creem el txt
    Txt txt;

    txt.size = 0;
    // Obtenim el tamany de la imatge
    int i = 6;
    while (received.data[i] != '&'){
        txt.size = txt.size * 10 + (received.data[i] - '0');
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

    txt.data.any = any;
    txt.data.mes = mes;
    txt.data.dia = dia;

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

    txt.hora.hora = hora;
    txt.hora.minut = minut;

    i++;

    // Obtenim el nom del fitxer
    int nameLength = received.length - i;
    txt.name = (char*)malloc((nameLength + 1) * sizeof(char));
    for (int k = 0; k < nameLength - 1; k++) {
        txt.name[k] = received.data[i+k];
    }
    txt.name[nameLength] = '\0';

    return txt;
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
    free(newImage.name);
    imagesList.images[index].size = newImage.size;
    imagesList.images[index].data = newImage.data;
    imagesList.images[index].hora = newImage.hora;

    printf("Nova imatge desada!\n");
    int numImatges = imagesList.numImages;
    printf("Tenim %d imatges guardades \n", numImatges);
    for (int i = 0; i < numImatges; i++) {
        printf("Imatge: %s \n", imagesList.images[i].name);
    }

    // Enviem la info de la imatge JPG a paquita
    MessageJPG messageJPG;
    messageJPG.type = JPG_TYPE;
    messageJPG.size = newImage.size;
    printf("Size: %d \n", messageJPG.size);
    if (msgsnd(queueId, &messageJPG, sizeof(messageJPG) - sizeof(long), 0) < 0){
        printf("Error al comunicar-me amb Paquita!\n");
    }

    // Desbloquejem el semafor
    pthread_mutex_unlock(&mutexLlistaImatges);

}

void addNewTxt(Txt newTxt){
    // Esperem al semafor i el bloquejem
    pthread_mutex_lock(&mutexLlistaTxts);

    int index = txtList.numTxt;
    txtList.numTxt++;
    txtList.txts = realloc(txtList.txts, txtList.numTxt * sizeof(Txt));

    // Copiem els valors de la nova imatge
    txtList.txts[index].name = (char*)malloc(sizeof(char) * strlen(newTxt.name));
    txtList.txts[index].name = strcpy(txtList.txts[index].name, newTxt.name);
    free(newTxt.name);
    txtList.txts[index].size = newTxt.size;
    txtList.txts[index].data = newTxt.data;
    txtList.txts[index].hora = newTxt.hora;

    printf("Nou txt desat!\n");
    int numTxts = txtList.numTxt;
    printf("Tenim %d txt guardats \n", numTxts);
    for (int i = 0; i < numTxts; i++) {
        printf("Txt: %s \n", txtList.txts[i].name);
    }

    // Enviem la info de la imatge JPG a paquita
    MessageTXT messageTXT;
    messageTXT.type = TXT_TYPE;
    messageTXT.length = strlen(txtList.txts[index].name);
    for (int i = 0; i < messageTXT.length; i++) {
        messageTXT.filename[i] = txtList.txts[index].name[i];
    }
    messageTXT.filename[messageTXT.length] = '\0';
    if (msgsnd(queueId, &messageTXT, sizeof(messageTXT) - sizeof(long), 0) < 0){
        printf("Error al comunicar-me amb Paquita!\n");
    }

    // Desbloquejem el semafor
    pthread_mutex_unlock(&mutexLlistaTxts);
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
        mcgruderName = (char*)malloc(sizeof(char) * strlen(mcGrudersList.mcgruders[index].telescopeName) + 1);
        mcgruderName = strcpy(mcgruderName, mcGrudersList.mcgruders[index].telescopeName);
        mcgruderName[strlen(mcgruderName) - 1] = '\0';
    }

    // Desbloquejem el semafor
    pthread_mutex_unlock(&mutexLlistaMcGruders);

    return mcgruderName;
}