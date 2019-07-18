//
// Created by xosel on 25/06/2019.
//

// Llibreries
#include "lionel_start.h"
#include "lionel_program.h"
#include "paquita.h"

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
        close(fdConfig); // Tanquem el socket perque no s'ha pogutt llegir correctament
        return EXIT_CODE_CONFIG;
    }
    // Tanquem el socket perque ja hem llegit l'arxiu
    close(fdConfig);

    mostraMissatgeStarting();

    if (configure()){
        mostraErrorStartConnection();
        return EXIT_CODE_CONNECTION;
    }else{
        // configurem les senyals per a quan ens tanquen Lionel sobtadament
        configureKills();
        // Creem la cua de missatges amb la que ens comunicarem amb paquita
        int queueError = createMessageQueue(argv[1]);
        if(queueError){
            mostraErrorCreacioQueue();
        }

        pidPaquita = fork();
        switch (pidPaquita){
            case -1:
                // Error
                mostraErrorArrencarPaquita();
                alliberaConfiguracio();
                break;
            case 0:
                // Fill (paquita) -----------------
                paquita();
                break;
            default:
                // Lionel (main) ------------------
                // Acceptem mcgruders
                acceptMcGruders();
                break;
        }
    }


    return EXIT_SUCCESS;
}