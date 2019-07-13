//
// Created by xosel on 20/06/2019.
//

// Llibreries
#include "mcgruder_connection.h"

// Funcions
int connecta(){
    mostraMissatgeConnecting();

    fdLionel = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (fdLionel < 0){
        // Alliberem la configuracio
        alliberaConfiguracio();
        return 1;
    }else{
        //memset(&socketConfig, 0, sizeof(socketConfig));
        bzero(&socketConfig, sizeof(socketConfig));

        socketConfig.sin_family = AF_INET;
        socketConfig.sin_port = htons(configuracio.port);
        socketConfig.sin_addr.s_addr = inet_addr(configuracio.ip);
        //inet_aton(configuracio.ip, &socketConfig.sin_addr.socketConfig);

        if (connect(fdLionel, (void*)&socketConfig, sizeof(socketConfig)) < 0){
            alliberaConfiguracio();
            mostraErrorConnexio();
            return 1;
        }else{
            return 0;
        }
    }
}
void desconnecta(){
    int lionelDOwn = enviaTramaDesconnexio();
    if (lionelDOwn){
        desconnectaForsaBruta();
    }else{
        Trama disconnectionTrama = receiveTrama();
        if (disconnectionTrama.length < 0 || strcmp(disconnectionTrama.header, HEADER_DISCONNECTION_RESPONSE_OK) == 0){
            if (disconnectionTrama.length > 0){
                free(disconnectionTrama.header);
                free(disconnectionTrama.data);
            }

            // Lionel ens ha dit que li sembla be que ens desconnectem
            desconnectaForsaBruta();

        }else{
            if (strcmp(disconnectionTrama.header, HEADER_DISCONNECTION_RESPONSE_KO) == 0){
                // Lionel ens ha dit que no li sembla be que ens desconnectem, que fem?
                // todo: QUE CONY FEM QUAN LIONEL ENS DIU QUE NO ENS PODEM DESCONNECTAR?¿?¿
            }
        }
    }

}

void desconnectaForsaBruta(){
    printf("MARXEM!\n");
    // tanquem el FD amb Lionel
    close(fdLionel);
    mostraMissatgeDisconnecting();
    alliberaConfiguracio();

    exit(EXIT_SUCCESS);
}