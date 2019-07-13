//
// Created by xosel on 19/06/2019.
//

// Llibreries
#include "mcgruder_configReader.h"

// Funcions
int existsFile(char* path){
    return open(path, O_RDONLY);
}

Configuracio readConfiguracio(int fd){
    Configuracio conf;

    char* cadena;
    int resp;
    int error = 0;

    cadena = (char *) malloc(sizeof(char));
    resp = llegirCadena(fd, '\n', cadena);

    if (resp < 0) {
        free(cadena);
        error++;
    }

    if (error == 0){
        conf.telescopeName = cadena;

        cadena = (char *) malloc(sizeof(char));
        resp = llegirCadena(fd, '\n', cadena);

        if (resp < 0) {
            free(cadena);
            free(conf.telescopeName);
            error++;
        }

        if (error == 0){
            //conf.time = strtol;
            conf.time = atoi(cadena);

            cadena = (char *) malloc(sizeof(char));
            resp = llegirCadena(fd, '\n', cadena);

            if (resp < 0) {
                free(cadena);
                free(conf.telescopeName);
                error++;
            }

            if (error == 0){
                conf.ip = cadena;

                cadena = (char *) malloc(sizeof(char));
                resp = llegirCadena(fd, '\n', cadena);

                if (resp < 0) {
                    free(cadena);
                    free(conf.telescopeName);
                    free(conf.ip);
                    error++;
                }

                if (error == 0){
                    conf.port = atoi(cadena);

                    resp = read(fd, cadena, sizeof(char));
                    if (resp != 0){
                        // Ha llegit algun byte i el fitxer ja hauria d'haver acabat
                        free(cadena);
                        free(conf.telescopeName);
                        free(conf.ip);
                        error++;
                    }

                }
            }
        }
    }


    if ((conf.time <= 0 || conf.port <= 0) && error == 0){
        free(cadena);
        free(conf.telescopeName);
        free(conf.ip);
        error++;
    }


    if (error == 0){
        free(cadena);
        return conf;
    }else{
        // Hi ha algun error, per tant ho marcarem amb un -1 al temps
        // ja que es un valor impossible i segons l'enunciat podem suposar que
        // el format de l'arxiu sera correcte
        mostraErrorFormatArxiuConfiguracio();
        conf.time = -1;
        return conf;
    }
}

int llegirCadena(int fd, char limit, char *cadena) {
    char c;
    char *tmp;
    int k = 0;
    ssize_t bytes;

    cadena[k++] = '\0';

    bytes = read(fd, &c, sizeof(char));
    if (bytes == 0) {
        return -1;
    }

    while(c != limit && (tmp = realloc(cadena, sizeof(char) + k)) != NULL) {
        cadena = tmp;
        cadena[k-1] = c;
        cadena[k++] = '\0';

        bytes = read(fd, &c, sizeof(char));
        if (bytes == 0) {
            free(tmp);
            return -1;
        }
    }


    return 0;
}
