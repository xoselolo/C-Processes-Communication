//
// Created by xosel on 22/06/2019.
//

// Llibreries
#include "mcgruder_scanner.h"

void desperta(){
    // Comencem per el path /files
    pathAcumulat = (char*)malloc(strlen(DIR_PATH) * sizeof(char));
    pathAcumulat = strcpy(pathAcumulat, DIR_PATH);

    parseDirectory();

    // Llancem una senyal d'alarma dintre de configuracio.time segons
    alarm(configuracio.time);

    // Mostrem el missatge de que estem esperant a que passi el temps d'espera
    mostraMissatgeWaiting();
    free(pathAcumulat);
}

void parseDirectory(){

    DIR* directory = NULL;

    // Quan despertem obrim el directori
    mostraMissatgeExploringDirectory(pathAcumulat);
    int open = openDirectory(&directory, pathAcumulat);
    if(open == -1){
        // Error, no s'ha pogut obrir el directori
        mostraErrorOberturaDirectori(pathAcumulat);
        free(pathAcumulat);
        desconnecta();

    }else{
        // Parsegem el directori
        mostraMissatgeTestingFiles();
        saltaBasura(directory);

        struct dirent* ent = readdir(directory);
        if (ent == NULL){
            printf("No hi ha arxius!\n");
            // llegim una trama per saber si el fd segueix obert o no, si no ho esta vol dir que Lionel ha caigut, i ens desconnectem
            // Si hi ha arxius, ens adonarem quan estiguem enviant trames del primer arxiu, ja que
            // al enviar la metadata el write valdra < 0
            /*char type;
            int disconnected = read(fdLionel, &type, sizeof(char));
            if (disconnected < 0){
                desconnectaForsaBruta();
            }else{
                // todo: CAL REVISAR ( --> MALE <-- )
                if (disconnected == 0){
                    // NO hem rebut res
                    printf("Lionel segueix running!\n");
                }else{
                    // Hem rebut una trama, i ha de ser la de desconnexio perque no hem enviat res!
                    printf("Crec que hem rebut una trama de desconnexio\n");
                }
            }*/
        }
        while (ent != NULL){
            // Enviem l'arxiu
            if (ent->d_name[0] != '.'){
                int errorSendFile = sendFile(ent);

                if (errorSendFile > 0){
                    // Error al enviar l'arxiu (Lionel ha caigut (1) o be el checksum es incorrecte(2))
                    mostraErrorEnviarFitxer(ent->d_name);

                    if (errorSendFile == 1){ // connection error
                        // Alliberem memoria i ens desconnectem tancant el FD i matant el proces
                        free(ent);
                        free(directory);
                        desconnectaForsaBruta();
                    }
                }
            }

            // Llegim el seguent arxiu (si hi ha, sino sera NULL)
            ent = readdir(directory);
        }

        //free(ent);
    }

    free(directory);



}

int openDirectory(DIR** directory, char* pathAcumulat){
    *directory = opendir(pathAcumulat);

    if (*directory == NULL){
        return -1;
    }else{
        return 0;
    }
}

void saltaBasura(DIR* directory){
    readdir(directory);
    readdir(directory);
}

int sendFile(struct dirent* ent) {
    char* previousPath;
    int imageError = 0;
    int txtError = 0;

    mostraMissatgeFileName(ent->d_name);

    int errorType = checkType(ent);
    switch (errorType){
        case FILE_TYPE_IMAGE:
            // Arxiu enviat perfectament!

            mostraMissatgeSendingFile(ent->d_name);

            imageError = sendImage(ent);

            if (imageError == 0){
                // Imatge enviada satisfacrtoriament
                mostraMissatgeFileSent();
                // Eliminem l'arxiu
                deleteFile(ent->d_name);
                return 0;
            }else{
                // si no ha arribat la imatge no la borrem
                return imageError;
            }
            break;


        case FILE_TYPE_TXT:
            // Arxiu enviat perfectament!
            mostraMissatgeFileName(ent->d_name);
            mostraMissatgeSendingFile(ent->d_name);

            // todo : send TXT file
            txtError = sendTxt(ent); // todo

            if (txtError == 0){
                // Imatge enviada satisfacrtoriament
                mostraMissatgeFileSent();
                // Eliminem l'arxiu
                deleteFile(ent->d_name);
            }else{
                // si no ha arribat la imatge no la borrem
                return txtError;
            }
            return 0;
            break;


        case FILE_TYPE_FOLDER:
            // Ens guardem el directori en el que ens trobem
            previousPath = (char*)malloc(strlen(pathAcumulat) * sizeof(char));
            previousPath = strcpy(previousPath, pathAcumulat);

            pathAcumulat = strcat(pathAcumulat, ent->d_name);
            pathAcumulat = strcat(pathAcumulat, "/");
            //printf("Path acumulat: %s \n", pathAcumulat);

            // Com es un directori, fem la crida recursiva per a parsejar tot l'arbre de directoris
            parseDirectory();

            free(pathAcumulat);
            pathAcumulat = (char*)malloc(sizeof(char) * strlen(previousPath));
            pathAcumulat = strcpy(pathAcumulat, previousPath);

            // Eliminem el directori
            deleteFile(ent->d_name);
            return 0;

            break;
        case FILE_TYPE_NOTVALID:
            return 2;
            break;
    }

    return 0; // No error
}

int checkType (struct dirent* ent){
    if (ent->d_type == DT_DIR){
        // Es tracta d'un directori
        return FILE_TYPE_FOLDER;
    }else{
        // Es tracta d'un fitxer
        // veiem quina extensio te
        char* extension = getExtensionArxiu(ent);
        if (strcmp(extension, FILE_EXTENSION_IMAGE) == 0){
            return FILE_TYPE_IMAGE;
        }else if (strcmp(extension, FILE_EXTENSION_TXT) == 0){
            return FILE_TYPE_TXT;
        }
        return FILE_TYPE_NOTVALID;
    }
}

char* getExtensionArxiu(struct dirent* ent){
    char* extension = (char*)malloc(sizeof(char) * 4);
    int initial = strlen(ent->d_name) - 4;
    for (int i = 0; i < 4; i++) {
        extension[i] = ent->d_name[initial + i];
    }
    return extension;
}

int getSizeArxiu(char* pathArxiu){

    // obrim el fitxer
    int fdArxiu = open(pathArxiu, O_RDONLY);
    if (fdArxiu > 0){
        int bytes = lseek(fdArxiu, 0, SEEK_END);
        close(fdArxiu);
        return bytes;
    }else{
        return -1;
    }
}

Data getDataActual(){

    Data data;
    time_t rawtime;
    struct tm *info;

    time(&rawtime);
    info = gmtime(&rawtime);

    data.any = info->tm_year + 1900;
    data.mes = info->tm_mon + 1;
    data.dia = info->tm_mday;

    return data;
}

Hora getHoraActual(){

    Hora hora;
    time_t rawtime;
    struct tm *info;

    time(&rawtime);
    info = gmtime(&rawtime);

    hora.hora = (info->tm_hour+LOCALTIME)%24;
    hora.minut = info->tm_min;

    return hora;
}

char* toStringDataHoraMetadata(Data dataActual, Hora horaActual){
    char* string = (char*)malloc(20 * sizeof(char));
    int length = sprintf(string, "%d-%d-%d %d:%d", dataActual.any, dataActual.mes, dataActual.dia, horaActual.hora, horaActual.minut);
    string[length] = '\0';

    return string;
}

char* formaDataMetadata(char tipus[4], int size, char* dataHoraString, char* filename){
    char* string = (char*)malloc(50 * sizeof(char));
    int length = sprintf(string, "[%s&%d&%s&%s]", tipus, size, dataHoraString, filename);
    string[length] = '\0';

    return string;
}

int sendImage(struct dirent* ent){
    // construim el path del fitxer
    char* pathArxiu = (char*)malloc((strlen(pathAcumulat) + strlen(ent->d_name)) * sizeof(char));
    pathArxiu = strcat(pathArxiu, pathAcumulat);
    pathArxiu = strcat(pathArxiu, ent->d_name);

    int size = getSizeArxiu(pathArxiu);
    if (size < 0){
        return 3;
    }
    Data dataActual = getDataActual();
    Hora horaActual = getHoraActual();

    Trama imageTrama;
    imageTrama.type = TYPE_SENDFILE;
    imageTrama.header = HEADER_SENDFILE_METADATA;

    char* dataHoraString = toStringDataHoraMetadata(dataActual, horaActual);

    // char* imageTramaData = formaDataMetadata(".jpg", size, dataHoraString, pathArxiu); todo: nom de l'arxiu amb path
    char* imageTramaData = formaDataMetadata(".jpg", size, dataHoraString, ent->d_name);
    int dataLength = strlen(imageTramaData);
    free(dataHoraString); // Alliberem la string on hem construit la data i hora (i que ja hem copiat)

    imageTrama.length = (short) dataLength;
    imageTrama.data = (char*)malloc(dataLength * sizeof(char));
    imageTrama.data = strcpy(imageTrama.data, imageTramaData);
    free(imageTramaData); // Alliberem la string on hem construit el camp data (i que ja hem copiat)

    int disconnection = sendTrama(imageTrama);
    free(imageTrama.data);

    if (disconnection){
        return 1; // Error de connexio
    }

    int fdArxiu = open(pathArxiu, O_RDONLY);

    if (fdArxiu < 0){
        return 4;
    } else{
        int bytesSent = 0;
        int bytesRead = 0;
        char* aux;

        imageTrama.type = TYPE_SENDFILE;
        imageTrama.header = HEADER_SENDFILE_DATA;

        while (bytesSent < size){
            int bytesRestantes = size - bytesSent;
            if (bytesRestantes < FILE_TRAMA_MAXSIZE){
                aux = (char*)malloc(bytesRestantes * sizeof(char));
                bytesRead = read(fdArxiu, aux, bytesRestantes * sizeof(char));
            }else{
                aux = (char*)malloc(FILE_TRAMA_MAXSIZE * sizeof(char));
                bytesRead = read(fdArxiu, aux, FILE_TRAMA_MAXSIZE);
            }

            if (bytesRead < 0){
                return 4;
            }else{
                // Copiem el length i la data de la trama
                imageTrama.length = (short) bytesRead;
                imageTrama.data = (char*)malloc(sizeof(char) * bytesRead);
                imageTrama.data = strcpy(imageTrama.data, aux);

                // Enviem la trama
                int disconnected = sendTrama(imageTrama);
                if (disconnected){
                    close(fdArxiu);
                    return 1;
                } else{
                    bytesSent += bytesRead;
                }
            }
        }

        close(fdArxiu);


        // todo: checksum --> PROVISIONAL



        Trama checkshumTrama;
        checkshumTrama.type = TYPE_SENDFILE;
        checkshumTrama.header = HEADER_SENDFILE_ENDFILE;
        //checkshumTrama.length = (short) strlen("[12345678901234567890123456789012]");
        checkshumTrama.length = (short) strlen(HEADER_SENDFILE_RESPONSE_OK_IMAGE);
        checkshumTrama.data = (char*)malloc(checkshumTrama.length * sizeof(char));
        // Checksum provisional
        checkshumTrama.data = strcpy(imageTrama.data, HEADER_SENDFILE_RESPONSE_OK_IMAGE);

        int disconected = sendTrama(checkshumTrama);
        if (disconected){
            return 1;
        }else{
            checkshumTrama = receiveTrama();

            //int solucio = -1;
            int solucio = 0;

            if (checkshumTrama.length < 0){
                printf("PROBLEMA 1\n");
                solucio =  1;
            }else{
                //mostraTrama(checkshumTrama);
                printf("STRL : %d \n", (int)strlen(checkshumTrama.header));
                if (checkshumTrama.type != TYPE_SENDFILE){
                    printf("PROBLEMA 2\n");
                    solucio = 6;
                }

                if (strcmp(checkshumTrama.header, HEADER_SENDFILE_RESPONSE_OK_IMAGE) == 0){
                    printf("CORRECTE\n");
                    solucio = 0;
                }

                if (strcmp(checkshumTrama.header, HEADER_SENDFILE_RESPONSE_KO_IMAGE) == 0){
                    printf("PROBLEMA 3\n");
                    solucio =  2;
                }

                if(solucio < 0){
                    printf("PROBLEMA 4\n");
                    solucio = 6;
                }
            }

            return solucio;
        }
    }

    return 0;
}

int sendTxt(struct dirent* ent){
    // todo
    return 0;
}

void deleteFile(char *fileName) {
    // Eliminem l'arxiu
    char* filePath = (char*)malloc(sizeof(char) * (strlen(pathAcumulat) + strlen(fileName)));
    sprintf(filePath, "%s%s", pathAcumulat, fileName);
    remove(filePath);
}