//
// Created by xosel on 13/07/2019.
//

//#include "checksum.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define PIPE_READ 0
#define PIPE_WRITE 1

//char* makeChecksum(char* filename){
int main(int argc, char** argv){
    int status;
    int sonStoped;

    int fd_pipeFillToPare[2];
    int fd_pipePareToFill[2];
    if (pipe(fd_pipeFillToPare) < 0 || pipe(fd_pipePareToFill) < 0){
        printf("Error al crear la pipe!\n");
        exit(-1);
    }


    int sonPID = fork();
    switch (sonPID){
        case -1:
            // Error
            printf("Error al crear el fork!\n");
            exit(-1);
            break;
        case 0:
            printf("Soc el fill!\n");
            // Tanquem fd de les pipes que no utilitzarem
            close(fd_pipeFillToPare[PIPE_READ]);
            close(fd_pipePareToFill[PIPE_WRITE]);

            int num1 = 1, num2;

            printf("Fill envia 1\n");
            write(fd_pipeFillToPare[PIPE_WRITE], &num1, sizeof(int));

            read(fd_pipePareToFill[PIPE_READ], &num2, sizeof(int));
            printf("Fill ha llegit %d \n", num2);

            // todo: close fds

            _exit(0);
            // Fill
            break;
        default:
            // Pare

            close(fd_pipeFillToPare[PIPE_WRITE]);
            close(fd_pipePareToFill[PIPE_READ]);

            int num1Pare, num2Pare = 2;

            read(fd_pipeFillToPare[PIPE_READ], &num1Pare, sizeof(int));
            printf("Pare ha llegit %d \n", num1Pare);

            printf("Pare ha escrit 2 \n");
            write(fd_pipePareToFill[PIPE_WRITE], &num2Pare, sizeof(int));


            sonStoped = waitpid(sonPID, &status, WUNTRACED);

            close(fd_pipeFillToPare[PIPE_READ]);
            close(fd_pipePareToFill[PIPE_WRITE]);

            printf("Tot ha anat be!\n");

            exit(0);
            break;
    }
}