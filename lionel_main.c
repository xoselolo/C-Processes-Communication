//
// Created by xosel on 25/06/2019.
//

// Llibreries
#include "lionel_start.h"
#include "lionel_program.h"

// Constants
#define EXIT_CODE_START         1
#define EXIT_CODE_CONFIG        2
#define EXIT_CODE_CONNECTION    3

int main(int argc, char** argv){

    int fdConfig = start(argc, argv);
    if (fdConfig == -1){
        // Error en arguments
        return EXIT_CODE_START;
    }

    configuracio = readConfiguracio(fdConfig);
    if (configuracio.tempsF3 == -1){
        // Error en arxiu de configuració
        close(fdConfig); // Tanquem el file descriptor de l'arxiu de configuració del lionel perque no s'ha pogutt llegir correctament
        return EXIT_CODE_CONFIG;
    }
    // Tanquem el file descriptor de l'arxiu de configuració del lionel perque ja l'hem llegit
    close(fdConfig);

    mostraMissatgeStarting();

    if (configure()){
        mostraErrorStartConnection();
        return EXIT_CODE_CONNECTION;
    }else{
        // configurem les senyals per a quan ens tanquen Lionel sobtadament
        configureKills();

        // Acceptem mcgruders
        acceptMcGruders();
    }


    return EXIT_SUCCESS;
}