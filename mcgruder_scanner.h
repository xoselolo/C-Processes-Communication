//
// Created by xosel on 22/06/2019.
//

#ifndef MCGRUDER_SCANNER_H
#define MCGRUDER_SCANNER_H

// Llibreries
#include <dirent.h>
//#include <sys/stat.h>
#include "mcgruder_connection.h"
#include "checksum.h"

// Constants
#define DIR_PATH "files/\0"
#define FILE_TYPE_IMAGE 1
#define FILE_TYPE_TXT 2
#define FILE_TYPE_FOLDER 3
#define FILE_TYPE_NOTVALID 4
#define FILE_EXTENSION_IMAGE ".jpg"
#define FILE_EXTENSION_TXT ".txt"
#define FILE_TRAMA_MAXSIZE 10000

// Capceleres

/**
 * Funcio que executara el mcgruder quan rebi una senyal d'alarma per a parsejar el directori mitjansant la funcio
 * parseDirectory()
 */
void desperta();

/**
 * Funcio que parseja el directori
 */
void parseDirectory();

/**
 * Funcio que retornara
 *      -1  quan hi hagi un problema de obertura del directori
 *       0 tot correcte
 */
int openDirectory(DIR** directory, char* pathAcumulat);

/**
 * Funcio per a llegir el '.' i el '..' d'un directori
 */
void saltaBasura(DIR* directory);

/**
 * Funcio que envia un arxiu a Lionel
 * @return
 *              0   :   Enviat correctament
 *              1   :   Connection Error
 *              2   :   Checksum Error
 *              3   :   Incorrect size
 *              4   :   Fd de lectura arxiu error
 *
 *              6   :   Trama desconeguda
 */
int sendFile(struct dirent* ent);

/**
 * Funcio que commprova el tipus d'arxiu del que es tracta
 * @return :
 *              1   :   Image
 *              2   :   Txt
 *              3   :   Folder
 *              4   :   Not valid
 */
int checkType (struct dirent* ent);

/**
 * Retorna una 'string' referent a la extensio de l'arxiu
 */
char* getExtensionArxiu(struct dirent* entry);

/**
 * Funcio que retorna el tamany en bytes de l'arxiu ent
 * @param ent : arxiu
 * @return bytes / -1 (no possible obrir arxiu)
 */
int getSizeArxiu(char* pathArxiu);

/**
 * Funcio que envia la imatge a Lionel
 * @return
 *              0   :   Enviat correctament
 *              1   :   Connection Error
 *              2   :   Checksum Error
 *              3   :   Open file Error
 */

/**
 * Funcio per a obtenir la data actual en la que estem rebent l'arxiu
 */
Data getDataActual();

/**
 * Funcio per a obtenir la hora actual en la que estem rebent l'arxiu
 */
Hora getHoraActual();

/**
 * Funcio que converteix la data i l'hora en una cadena de caracters per a la trama de metadata
 */
char* toStringDataHoraMetadata(Data dataActual, Hora horaActual);

/**
 * Funcio que a partir del tipus d'arxiu (@param tipus), el tamany de l'arxiu (@param size) i la data i l'hora en format
 * string (@param dataHoraString), retorna la string (char*) del camp data que haurem de posar a la trama de metadata
 * en l'enviament de l'arxiu
 */
char* formaDataMetadata(char tipus[4], int size, char* dataHoraString, char* filename);

/**
 * Funcio que envia la imatge a Lionel
 * @return
 *              0   :   Enviat correctament
 *              1   :   Connection Error
 *              2   :   Checksum Error
 *              3   :   Incorrect size
 *              4   :   Fd de lectura arxiu error
 *
 *              6   :   Trama desconeguda
 */
int sendImage(struct dirent* ent);

/**
 * Funcio que envia el .txt a Lionel
 * @return
 *              0   :   Enviat correctament
 *              1   :   Connection Error
 *              2   :   Checksum Error
 *              3   :   Incorrect size
 *              4   :   Fd de lectura arxiu error
 *
 *              6   :   Trama desconeguda
 */
int sendTxt(struct dirent* ent);

/**
 * Elimina l'arxiu del directori /files
 * @param fileName : nom de l'arxiu
 */
void deleteFile(char *fileName);

#endif //MCGRUDER_SCANNER_H
