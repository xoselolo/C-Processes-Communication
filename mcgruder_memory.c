//
// Created by xosel on 28/06/2019.
//

#include "mcgruder_memory.h"


void alliberaConfiguracio(){
    free(configuracio.telescopeName);
    free(configuracio.ip);
}