// Llibreries
#include "mcgruder_start.h"
#include "mcgruder_program.h"

// EXIT CODES of different types of error
#define EXIT_CODE_START     1
#define EXIT_CODE_CONFIG    2
#define EXIT_CODE_CONNECTION 3

int main(int argc, char** argv) {
    // Provem d'engegar el mcgruder
    int fdConfig = start(argc, argv);
    if(fdConfig == -1){
        // Error en numero arguments, arxiu, etc...
        return EXIT_CODE_START;
    }

    // Si l'engegament (arguments...) es correcta, llegim la configuracio
    configuracio = readConfiguracio(fdConfig);
    if (configuracio.time == -1){
        close(fdConfig);
        // Error en la lectura de l'arxiu
        return EXIT_CODE_CONFIG;
    }
    close(fdConfig);

    // Arxiu de configuracio correcte!
    mostraMissatgeStarting();

    // Configurem el McGruder per a que pari quan apretem Ctrl+C
    configure();

    // McGruder configurat
    // Ara el connectem amb Lionel
    if (connecta() != 0){
        // Error al establir la connexio amb Lionel
        mostraMissatgeConnectionFailed();
        return EXIT_CODE_CONNECTION;
    }

    // Connexio establerta, enviem trama de connexio a Lionel (teoric, perque ja estem connectats)
    int errorConnection = sendConnectionTrama();
    if (errorConnection == 1){
        // error a l'hora de connectar
        desconnectaForsaBruta();
    }else{
        // Connexio establerta amb exit
        run();
    }

    return EXIT_SUCCESS;
}
