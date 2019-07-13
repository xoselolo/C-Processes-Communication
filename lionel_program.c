//
// Created by xosel on 27/06/2019.
//

#include "lionel_program.h"

void acceptMcGruders(){
    socklen_t length = sizeof(socketConfig);
    while (1){
        mostraMissatgeWaiting();
        int fdNewMcgruder = accept(configuracio.fdSocket, (void*) &socketConfig, &length);
        if (fdNewMcgruder < 0){
            //mostraErrorNewMcgruder();
            mostraErrorConnexioSocket();
        }else{
            creaNouMcGruder(fdNewMcgruder);
        }
    }
}
