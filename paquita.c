//
// Created by xosel on 17/07/2019.
//

#include "paquita.h"

pthread_mutex_t mutexPaquitaJPG = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutexPaquitaTXT = PTHREAD_MUTEX_INITIALIZER;

void paquita(){
    signal(SIGINT, paquitaDesconnecta);
    signal(SIGTERM, paquitaDesconnecta);

    readersList.numReaders = 2;
    readersList.readers = (PaquitaReader*)malloc(2 * sizeof(PaquitaReader));

    readersList.readers[0].type = 3;
    pthread_create(&readersList.readers[BUSTIA_JPG].thread, NULL, readerFuncJPG, &queueId);

    readersList.readers[1].type = 2;
    pthread_create(&readersList.readers[BUSTIA_TXT].thread, NULL, readerFuncTXT, &queueId);

    pause();
}

void paquitaDesconnecta(){
    printf("Desconnectant readers...\n");
    for (int i = 0; i < readersList.numReaders; i++) {
        pthread_cancel(readersList.readers[i].thread);
    }
    free(readersList.readers);
    printf("Readers desconectats!\n");
}

void* readerFuncJPG(void* arg){
    // Obtenim el ID de la cua de missatges que hem rebut com a parametre de la funcio
    int queue = *((int*)arg);

    paquitaJPGinfo.numTotalImatges = 0;
    paquitaJPGinfo.totalKB = 0;

    while (1){
        MessageJPG messageJPG;
        if (msgrcv(queue, &messageJPG, sizeof(messageJPG) - sizeof(long), JPG_TYPE, 0) < 0){
            // error
            write(1, "Error al calcular a Paquita segons la imatge rebuda!\n", strlen("Error al calcular a Paquita segons la imatge rebuda!\n") *
                    sizeof(char));
        }else{
            // Esperem al semafor (McTavish) i el bloquejem
            pthread_mutex_lock(&mutexPaquitaJPG);

            tractaImatge(messageJPG);

            // Desbloquejem el semafor
            pthread_mutex_unlock(&mutexPaquitaJPG);
        }
    }
}

void* readerFuncTXT(void* arg){
    // Obtenim el ID de la cua de missatges que hem rebut com a parametre de la funcio
    int queue = *((int*)arg);

    paquitaTXTinfo.numTotalTxt = 0;
    paquitaTXTinfo.mitjanaConstelacionsPerArxiu = 1;

    while (1){
        MessageTXT messageTXT;
        if (msgrcv(queue, &messageTXT, sizeof(messageTXT) - sizeof(long), TXT_TYPE, 0) < 0){
            // error
            write(1, "Error al calcular a Paquita segons el text rebut!\n",
                    strlen("Error al calcular a Paquita segons el text rebut!\n") * sizeof(char));
        }else{
            // Esperem al semafor (McTavish) i el bloquejem
            pthread_mutex_lock(&mutexPaquitaTXT);

            tractaText(messageTXT);

            // Desbloquejem el semafor
            pthread_mutex_unlock(&mutexPaquitaTXT);
        }
    }
}


// Funcio de les imatges
void tractaImatge(MessageJPG messageJPG){
    paquitaJPGinfo.numTotalImatges++;
    paquitaJPGinfo.totalKB += ((float) messageJPG.size) / 1024;

    printf("Nova info de JPG: \n");
    printf("\t Num total imatges: %d \n", paquitaJPGinfo.numTotalImatges);
    printf("\t Num total KB: %.2f \n", paquitaJPGinfo.totalKB);
}

// Funcio dels texts
void tractaText(MessageTXT messageTXT){
    lastTxtInfo = initLastText();

    int length = strlen("files/") + messageTXT.length;
    //char* pathArxiu = (char*)malloc(length * sizeof(char));
    char* pathArxiu = strdup("files/");
    //pathArxiu = strcat(pathArxiu, pathAcumulat);
    pathArxiu = (char*)realloc(pathArxiu, (length + 1) * sizeof(char));
    pathArxiu = strcat(pathArxiu, messageTXT.filename);
    pathArxiu[length] = '\0';

    int fdArxiuConstelacions = open(pathArxiu, O_RDONLY);
    if (fdArxiuConstelacions < 0){
        printf("[PAQUITA] - No s'ha pogut obrir l'arxiu!\n");
    }else{
        int final = 0;
        while (final != 1){
            Constelacio constelacio;
            final = llegirConstelacio(fdArxiuConstelacions, &constelacio);
            // todo: lolo tractar constelacio
        }
    }

}

LastTxtInfo initLastText(){
    LastTxtInfo last;

    last.constelacions = NULL;
    last.numConstelacions = 0;
    last.mitjanaDensitats = 1;
    last.magnitudMAX = -1;
    last.magnitudMIN = -1;

    return last;
}

int llegirConstelacio(int fd, Constelacio* novaConstelacio){

    // leer una linia del arxivo,
    // 1 final
    // 0 no final

    return 0;
}

// Mitjanes
void actualitzaMitjanaConstelacions(LastTxtInfo last){
    paquitaTXTinfo.mitjanaConstelacionsPerArxiu = paquitaTXTinfo.mitjanaConstelacionsPerArxiu * paquitaTXTinfo.numTotalTxt;
    paquitaTXTinfo.numTotalTxt++;
    paquitaTXTinfo.mitjanaConstelacionsPerArxiu += last.numConstelacions;
    paquitaTXTinfo.mitjanaConstelacionsPerArxiu = paquitaTXTinfo.mitjanaConstelacionsPerArxiu / paquitaTXTinfo.numTotalTxt;
}

void actualitzaLast(Constelacio novaConstelacio){
    if(lastTxtInfo.magnitudMIN < 0){
        lastTxtInfo.magnitudMIN = novaConstelacio.magnitud;
        lastTxtInfo.magnitudMAX = novaConstelacio.magnitud;
    }else{
        if (novaConstelacio.magnitud < lastTxtInfo.magnitudMIN){
            lastTxtInfo.magnitudMIN = novaConstelacio.magnitud;
        }
        if (novaConstelacio.magnitud > lastTxtInfo.magnitudMAX){
            lastTxtInfo.magnitudMAX = novaConstelacio.magnitud;
        }
    }

    lastTxtInfo.mitjanaDensitats = lastTxtInfo.mitjanaDensitats * lastTxtInfo.numConstelacions;
    lastTxtInfo.mitjanaDensitats = lastTxtInfo.mitjanaDensitats + novaConstelacio.densitat;
    lastTxtInfo.numConstelacions++;
    lastTxtInfo.mitjanaDensitats = lastTxtInfo.mitjanaDensitats / lastTxtInfo.numConstelacions;

    int numConst = lastTxtInfo.numConstelacions;
    if (numConst == 1){
        lastTxtInfo.constelacions = (Constelacio*)malloc(sizeof(Constelacio));
        for (int i = 0; i < 3; i++) {
            lastTxtInfo.constelacions[0].codi[i] = novaConstelacio.codi[i];
        }
        lastTxtInfo.constelacions[0].densitat = novaConstelacio.densitat;
        lastTxtInfo.constelacions[0].magnitud = novaConstelacio.magnitud;
    }else{
        lastTxtInfo.constelacions = (Constelacio*)realloc(lastTxtInfo.constelacions, numConst * sizeof(Constelacio));
        for (int i = 0; i < 3; i++) {
            lastTxtInfo.constelacions[numConst - 1].codi[i] = novaConstelacio.codi[i];
        }
        lastTxtInfo.constelacions[numConst - 1].densitat = novaConstelacio.densitat;
        lastTxtInfo.constelacions[numConst - 1].magnitud = novaConstelacio.magnitud;
    }
}