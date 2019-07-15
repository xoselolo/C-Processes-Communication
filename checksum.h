//
// Created by xosel on 13/07/2019.
//

#ifndef MCGRUDER_CHECKSUM_H
#define MCGRUDER_CHECKSUM_H

#include <fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>

char* makeChecksum(char* filename);


#endif //MCGRUDER_CHECKSUM_H
