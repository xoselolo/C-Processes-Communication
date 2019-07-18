//
// Created by xosel on 17/07/2019.
//

#ifndef MCGRUDER_PAQUITA_H
#define MCGRUDER_PAQUITA_H

#include "lionel_types.h"

/**
 * Configura les variables globals, threads...
 */
void paquita();

void paquitaDesconnecta();

void* readerFuncJPG(void* arg);
void* readerFuncTXT(void* arg);

void tractaImatge(MessageJPG messageJPG);

void tractaText(MessageTXT messageTXT);
LastTxtInfo initLastText();
int llegirConstelacio(int fd, Constelacio* novaConstelacio);
void actualitzaMitjanaConstelacions(LastTxtInfo last);
void actualitzaLast(Constelacio novaConstelacio);

#endif //MCGRUDER_PAQUITA_H
