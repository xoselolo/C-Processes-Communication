//
// Created by xosel on 25/06/2019.
//

#include "lionel_start.h"

extern pthread_mutex_t mutexLlistaMcGruders;
extern pthread_mutex_t mutexLlistaImatges;
extern pthread_mutex_t mutexLlistaTxts;

int start(int argc, char** argv){
    // Controlem que s'hagin introduit el numero correcte d'arguments
    if (controlaNumeroArguments(argc)){
        // Error en el numero d'arguments
        mostraErrorNumeroArguments();
        return -1;
    }

    // Si no hi ha hagut cap error en el numero d'arguments intentarem configurar el mcGruder
    int fdConfigFile = existsFile(argv[1]);
    if (fdConfigFile == -1){
        // Error en la opertura del fitxer de configuracio
        mostraErrorFitxerConfiguracioNoTrobat();
        return -1;
    }

    // L'arxiu existeix i ha estat obert
    return fdConfigFile;
}

int controlaNumeroArguments(int argc){
    if(argc == 2){
        return 0;
    }else{
        return 1;
    }
}

int configure(){
    configuracio.fdSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (configuracio.fdSocket < 0){
        alliberaConfiguracio();
        return 1;

    }else{

        //memset(&socketConfig, 0, sizeof(socketConfig));
        bzero(&socketConfig, sizeof(socketConfig));

        socketConfig.sin_family = AF_INET;
        socketConfig.sin_port = htons(configuracio.portMcGruders);
        socketConfig.sin_addr.s_addr = inet_addr(configuracio.ip);
        //inet_aton(configuracio.ip, &socketConfig.sin_addr.socketConfig);

        if (bind(configuracio.fdSocket, (void*) &socketConfig, sizeof(socketConfig)) < 0){
            alliberaConfiguracio();
            return 1;
        }else{
            listen(configuracio.fdSocket, BACKLOG);

            configureInitialMcGruders();
            configureInitialFiles();
            return 0;
        }
    }
}

void configureInitialMcGruders(){
    // Inicialitzem els valors
    mcGrudersList.numMcGrudersConnected = 0;
    mcGrudersList.mcgruders = NULL;

    // Creem el semafor per a que els servidors dedicats no es trepitgin la memoria
    pthread_mutex_init(&mutexLlistaMcGruders, NULL);
}

void configureInitialFiles(){
    // inicialitzem els valors de les imatges
    imagesList.numImages = 0;
    imagesList.images = NULL;

    // inicialitzem els valors dels .txt
    txtList.numTxt = 0;
    txtList.txts = NULL;

    // creem els semafors per a imatges i txts independents entre si, per fer que dos mcgruders puguin
    // afegir una imatge i un txt alhora, respectivament
    pthread_mutex_init(&mutexLlistaImatges, NULL);
    pthread_mutex_init(&mutexLlistaTxts, NULL);
}

void configureKills(){
    signal(SIGINT, desconnecta);
    signal(SIGTERM, desconnecta);
}