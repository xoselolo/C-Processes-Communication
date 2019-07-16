//
// Created by xosel on 22/06/2019.
//

// Llibreries
#include "mcgruder_scanner.h"

void desperta(){
    // Comencem per el path /files
    //pathAcumulat = (char*)malloc((strlen(DIR_PATH) + 1) * sizeof(char));
    pathAcumulat = strdup(DIR_PATH);

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
            printf("No hi ha arxius!\n"); // todo: canviar per un WRITE
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
                        //free(directory);
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
    int prevLength = 0;

    mostraMissatgeFileName(ent->d_name);

    int errorType = checkType(ent);
    switch (errorType){
        case FILE_TYPE_IMAGE:
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
            mostraMissatgeSendingFile(ent->d_name);

            txtError = sendTxt(ent);

            if (txtError == 0){
                // Imatge enviada satisfacrtoriament
                mostraMissatgeFileSent();
                // Eliminem l'arxiu
                deleteFile(ent->d_name);
                return 0;
            }else{
                // si no ha arribat la imatge no la borrem
                return txtError;
            }
            break;


        case FILE_TYPE_FOLDER:
            // Ens guardem el directori en el que ens trobem
            prevLength = strlen(pathAcumulat);
            previousPath = strdup(pathAcumulat);

            pathAcumulat = realloc(pathAcumulat, (prevLength + strlen(ent->d_name) + 1) * sizeof(char));
            pathAcumulat = strcat(pathAcumulat, ent->d_name);
            pathAcumulat = strcat(pathAcumulat, "/\0");

            // Com es un directori, fem la crida recursiva per a parsejar tot l'arbre de directoris
            parseDirectory();

            // Eliminem el directori
            deleteFile(pathAcumulat);

            free(pathAcumulat);
            pathAcumulat = strdup(previousPath);
            //pathAcumulat = (char*)malloc(sizeof(char) * strlen(previousPath));
            //pathAcumulat = strcpy(pathAcumulat, previousPath);
            free(previousPath);
            return 0;

            break;
        case FILE_TYPE_NOTVALID:
            mostraErrorTipusDeFitxer();
            deleteFile(ent->d_name);
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
            free(extension);
            return FILE_TYPE_IMAGE;
        }else if (strcmp(extension, FILE_EXTENSION_TXT) == 0){
            free(extension);
            return FILE_TYPE_TXT;
        }
        free(extension);
        return FILE_TYPE_NOTVALID;
    }
}

char* getExtensionArxiu(struct dirent* ent){
    char* extension = (char*)malloc(sizeof(char) * 5);
    int initial = strlen(ent->d_name) - 4;

    extension = strncpy(extension, ent->d_name + initial, 4);
    extension[4] = '\0';
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
    char* name = strdup(ent->d_name);
    int length = strlen(pathAcumulat) + strlen(name);
    //char* pathArxiu = (char*)malloc(length * sizeof(char));
    char* pathArxiu = strdup(pathAcumulat);
    //pathArxiu = strcat(pathArxiu, pathAcumulat);
    pathArxiu = (char*)realloc(pathArxiu, (length + 1) * sizeof(char));
    pathArxiu = strcat(pathArxiu, name);
    pathArxiu[length] = '\0';
    write(1, "PATH ARXIU: ", 12);
    write(1, pathArxiu, length);
    write(1, "\n", 1);

    int size = getSizeArxiu(pathArxiu);
    if (size < 0){
        printf("Error en size \n");
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
    //free(imageTramaData); // Alliberem la string on hem construit el camp data (i que ja hem copiat)

    int disconnection = sendTrama(imageTrama);
    //free(imageTrama.data);

    if (disconnection){
        printf("Error de connexio\n");
        return 1; // Error de connexio
    }

    int fdArxiu = open(pathArxiu, O_RDONLY);

    if (fdArxiu < 0){
        free(pathArxiu);
        printf("Error en apertura\n");
        return 4;
    } else{
        int bytesSent = 0;
        int bytesRead = 0;
        char* aux = NULL;

        imageTrama.type = TYPE_SENDFILE;
        imageTrama.header = HEADER_SENDFILE_DATA;

        while (bytesSent < size){
            int bytesRestantes = size - bytesSent;
            if (bytesRestantes < FILE_TRAMA_MAXSIZE){
                free(aux);
                aux = (char*)malloc((bytesRestantes+1) * sizeof(char));
                bytesRead = read(fdArxiu, aux, bytesRestantes * sizeof(char));
                aux[bytesRestantes] = '\0';
            }else{
                free(aux);
                aux = (char*)malloc((FILE_TRAMA_MAXSIZE+1) * sizeof(char));
                bytesRead = read(fdArxiu, aux, FILE_TRAMA_MAXSIZE);
                aux[FILE_TRAMA_MAXSIZE] = '\0';
            }

            if (bytesRead <= 0){
                //free(aux);
                //free(pathArxiu);
                printf("Error en lectura (connexion) \n");
                return 1;
            }else{
                // Copiem el length i la data de la trama
                imageTrama.length = (short) bytesRead;
                imageTrama.data = aux;
                //imageTrama.data = (char*)malloc(sizeof(char) * bytesRead);
                //imageTrama.data = strcpy(imageTrama.data, aux);
                //free(aux);

                // Enviem la trama
                int disconnected = sendTrama(imageTrama);
                //free(imageTrama.data);
                if (disconnected){
                    close(fdArxiu);
                    //free(pathArxiu);
                    printf("Error de connexio\n");
                    return 1;

                } else{
                    bytesSent += bytesRead;
                    mostraMissatgePercentatge(bytesSent, size, ent->d_name);
                }
            }
        }

        close(fdArxiu);


        // Calculem el checksum de l'arxiu
        char* checksum = makeChecksum(pathArxiu);
        printf("CHECKSUM: -%s- \n", checksum);

        free(pathArxiu);


        Trama checksumTrama;
        checksumTrama.type = TYPE_SENDFILE;
        checksumTrama.header = HEADER_SENDFILE_ENDFILE;
        checksumTrama.length = (short) 32; // todo -> preguntar al male si puc enviar CJECKSUM\0 per a rebreho tal qual i llegir 33 caracters directament per poder fer el strcmp a lionel
        //checksumTrama.data = (char*)malloc(32 * sizeof(char));
        //checksumTrama.data = strcpy(checksumTrama.data, checksum);
        checksumTrama.data = strdup(checksum);
        //free(checksum);

        int disconected = sendTrama(checksumTrama);
        //free(checksumTrama.data);
        if (disconected){
            printf("Error de connexio\n");
            return 1;
        }else{
            checksumTrama = receiveTrama();

            //int solucio = -1;
            int solucio = 0;

            if (checksumTrama.length < 0){
                printf("Error de connexio 2\n");
                solucio =  1;
            }else{
                //mostraTrama(checkshumTrama);
                //printf("STRL : %d \n", (int)strlen(checkshumTrama.header));
                if (checksumTrama.type != TYPE_SENDFILE){
                    printf("Error de TIPUS\n");
                    solucio = 6;
                }

                if (strcmp(checksumTrama.header, HEADER_SENDFILE_RESPONSE_OK_IMAGE) == 0){
                    printf("TOT OK\n");
                    solucio = 0;
                }

                if (strcmp(checksumTrama.header, HEADER_SENDFILE_RESPONSE_KO_IMAGE) == 0){
                    printf("KO\n");
                    solucio =  2;
                }
            }

            return solucio;
        }
    }

    return 0;
}

int sendTxt(struct dirent* ent){
    // construim el path del fitxer
    char* name = strdup(ent->d_name);
    int length = strlen(pathAcumulat) + strlen(name);
    //char* pathArxiu = (char*)malloc(length * sizeof(char));
    char* pathArxiu = strdup(pathAcumulat);
    //pathArxiu = strcat(pathArxiu, pathAcumulat);
    pathArxiu = (char*)realloc(pathArxiu, (length + 1) * sizeof(char));
    pathArxiu = strcat(pathArxiu, name);
    pathArxiu[length] = '\0';
    write(1, "PATH ARXIU: ", 12);
    write(1, pathArxiu, length);
    write(1, "\n", 1);

    int size = getSizeArxiu(pathArxiu);
    if (size < 0){
        printf("Error en size \n");
        return 3;
    }
    Data dataActual = getDataActual();
    Hora horaActual = getHoraActual();

    Trama imageTrama;
    imageTrama.type = TYPE_SENDFILE;
    imageTrama.header = HEADER_SENDFILE_METADATA;

    char* dataHoraString = toStringDataHoraMetadata(dataActual, horaActual);

    // char* imageTramaData = formaDataMetadata(".jpg", size, dataHoraString, pathArxiu);
    char* imageTramaData = formaDataMetadata(".txt", size, dataHoraString, ent->d_name);
    int dataLength = strlen(imageTramaData);
    free(dataHoraString); // Alliberem la string on hem construit la data i hora (i que ja hem copiat)

    imageTrama.length = (short) dataLength;
    imageTrama.data = (char*)malloc(dataLength * sizeof(char));
    imageTrama.data = strcpy(imageTrama.data, imageTramaData);
    //free(imageTramaData); // Alliberem la string on hem construit el camp data (i que ja hem copiat)

    int disconnection = sendTrama(imageTrama);
    //free(imageTrama.data);

    if (disconnection){
        printf("Error de connexio\n");
        return 1; // Error de connexio
    }

    int fdArxiu = open(pathArxiu, O_RDONLY);

    if (fdArxiu < 0){
        free(pathArxiu);
        printf("Error en apertura\n");
        return 4;
    } else{
        // Podem enviar el contingut del fitxer --> todo
        free(imageTrama.data);

        imageTrama.type = TYPE_SENDFILE;
        imageTrama.header = HEADER_SENDFILE_DATA;
        imageTrama.length = (short) size;
        imageTrama.data = (char*)malloc(size * sizeof(char));
        read(fdArxiu, imageTrama.data, size * sizeof(char));

        //mostraTrama(imageTrama);

        disconnection = sendTrama(imageTrama);
        if (disconnection){
            printf("Error de connexio\n");
            return 1;
        }

        Trama fileOkTrama = receiveTrama();
        if (fileOkTrama.length < 0){
            printf("Error de connexio\n");
            return 1;
        }

        if (fileOkTrama.type == TYPE_DISCONNECTION){
            return 1;
        }

        if (fileOkTrama.type != TYPE_SENDFILE){
            return 3; // Trama desconeguda
        }

        // Sabem que es del tipus sendfile, mirem el header
        if (strcmp(fileOkTrama.header, HEADER_SENDFILE_RESPONSE_OK_TEXT) == 0){
            // L'arxiu s'ha enviat be
            deleteFile(pathArxiu);
            return 0;
        }else{
            return 2;
        }
    }
}

void deleteFile(char *fileName) {
    // Eliminem l'arxiu
    char* filePath = (char*)malloc(sizeof(char) * (strlen(pathAcumulat) + strlen(fileName)));
    sprintf(filePath, "%s%s", pathAcumulat, fileName);
    remove(filePath);
    free(filePath);
}