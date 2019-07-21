//
// Created by xosel on 20/06/2019.
//

// Llibreries
#include "mcgruder_program.h"

// Funcions i procediments
void run(){
    // configurem el senyal d'alarma per a que executi la funcio de parsejar el directori /files
    signal(SIGALRM, desperta);

    // Mostrem el primer missatge de waiting
    mostraMissatgeWaiting();
    // Llancem una senyal d'alarma dintre de configuracio.time segons
    alarm(configuracio.time);
    while (1){
        Trama lionelDownTrama = receiveTrama();
        if (lionelDownTrama.length < 0){
            // ha caigut lionel sobtadament
            desconnectaForsaBruta();
        }else{
            desconnectaForsaBruta(); // de moment
            // todo: tractar trama (ha de ser la de desconnexio, perque lionel no envia res a no ser que nosaltres ho
            // todo: estiguem esperant a dins de la funcio parseDirectory (a dins de desperta), a un altre scope
        }
    }
}