//
// Created by xosel on 25/06/2019.
//

#include "lionel_configReader.h"


// Funcions
int existsFile(char* path){
    return open(path, O_RDONLY);
}

Configuracio readConfiguracio(int fdConfig){
    Configuracio conf;

    char* cadena;
    int resp, error = 0;

    cadena = (char *) malloc(sizeof(char));
    resp = llegirCadena(fdConfig, '\n', cadena);

    if (resp < 0){
        free(cadena);
        error++;
    }

    if (error == 0){
        conf.ip = cadena;

        cadena = (char *) malloc(sizeof(char));
        resp = llegirCadena(fdConfig, '\n', cadena);

        if (resp < 0){
            free(cadena);
            free(conf.ip);
            error++;
        }

        if (error == 0){
            conf.portMcGruders = atoi(cadena);

            cadena = (char *) malloc(sizeof(char));
            resp = llegirCadena(fdConfig, '\n', cadena);

            if (resp < 0){
                free(cadena);
                free(conf.ip);
                error++;
            }

            if (error == 0){
                conf.portMcTavish = atoi(cadena);

                cadena = (char *) malloc(sizeof(char));
                resp = llegirCadena(fdConfig, '\n', cadena);

                if (resp < 0){
                    free(cadena);
                    free(conf.ip);
                    error++;
                }

                if (error == 0){
                    conf.tempsF3 = atoi(cadena);

                    resp = read(fdConfig, cadena, sizeof(char));
                    if (resp != 0){
                        // Ha llegit algun byte i el fitxer ja hauria d'haver acabat
                        free(cadena);
                        free(conf.ip);
                        error++;
                    }
                }
            }
        }
    }

    if ((conf.tempsF3 <= 0 || conf.portMcGruders <= 0 || conf.portMcTavish <= 0) && error == 0){
        free(cadena);
        free(conf.ip);
        error++;
    }

    if (error == 0){
        free(cadena);
        return conf;
    } else{
        // Hi ha error, marquem conf.tempsF3
        free(cadena);
        free(conf.ip);

        mostraErrorFormatArxiuConfiguracio();
        conf.tempsF3 = -1;
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

    while(c != limit && (tmp = realloc(cadena, sizeof(char) * k)) != NULL) {
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
