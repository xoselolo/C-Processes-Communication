//
// Created by xosel on 19/06/2019.
//

#include "mcgruder_start.h"

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
void configure(){
    signal(SIGINT, desconnecta);
    signal(SIGTERM, desconnecta);
}