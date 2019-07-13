all: McGruder Lionel

mcgruder_main.o: mcgruder_main.c mcgruder_start.h mcgruder_program.h
	gcc -c mcgruder_main.c -Wall -Wextra
mcgruder_start.o: mcgruder_start.c mcgruder_start.h mcgruder_configReader.h mcgruder_connection.h
	gcc -c mcgruder_start.c -Wall -Wextra
mcgruder_program.o: mcgruder_program.c mcgruder_program.h mcgruder_connection.h mcgruder_scanner.h
	gcc -c mcgruder_program.c -Wall -Wextra
mcgruder_scanner.o: mcgruder_scanner.c mcgruder_scanner.h mcgruder_connection.h
	gcc -c mcgruder_scanner.c -Wall -Wextra
mcgruder_configReader.o: mcgruder_configReader.c mcgruder_configReader.h mcgruder_screen.h
	gcc -c mcgruder_configReader.c -Wall -Wextra
mcgruder_connection.o: mcgruder_connection.c mcgruder_connection.h mcgruder_memory.h mcgruder_trama.h
	gcc -c mcgruder_connection.c -Wall -Wextra
mcgruder_screen.o: mcgruder_screen.c mcgruder_screen.h mcgruder_types.h
	gcc -c mcgruder_screen.c -Wall -Wextra
mcgruder_memory.o: mcgruder_memory.c mcgruder_memory.h
	gcc -c mcgruder_memory.c -Wall -Wextra
mcgruder_trama.o: mcgruder_trama.c mcgruder_trama.h mcgruder_screen.h
	gcc -c mcgruder_trama.c -Wall -Wextra
McGruder: mcgruder_main.o mcgruder_start.o mcgruder_program.o mcgruder_scanner.o mcgruder_configReader.o mcgruder_connection.o mcgruder_screen.o mcgruder_memory.o mcgruder_trama.o
	gcc mcgruder_main.o mcgruder_start.o mcgruder_program.o mcgruder_scanner.o mcgruder_configReader.o mcgruder_connection.o mcgruder_screen.o mcgruder_memory.o mcgruder_trama.o -o McGruder -Wall -Wextra

lionel_main.o: lionel_main.c lionel_start.h lionel_program.h
	gcc -c lionel_main.c -lpthread -Wall -Wextra
lionel_start.o: lionel_start.c lionel_start.h lionel_configReader.h lionel_connection.h
	gcc -c lionel_start.c -lpthread -Wall -Wextra
lionel_program.o: lionel_program.c lionel_program.h lionel_connection.h
	gcc -c lionel_program.c -lpthread -Wall -Wextra
lionel_connection.o: lionel_connection.c lionel_connection.h lionel_screen.h lionel_memory.h lionel_trama.h
	gcc -c lionel_connection.c -lpthread -Wall -Wextra
lionel_configReader.o: lionel_configReader.c lionel_configReader.h lionel_screen.h
	gcc -c lionel_configReader.c -lpthread -Wall -Wextra
lionel_memory.o: lionel_memory.c lionel_memory.h lionel_types.h
	gcc -c lionel_memory.c -lpthread -Wall -Wextra
lionel_trama.o: lionel_trama.c lionel_trama.h lionel_types.h lionel_screen.h
	gcc -c lionel_trama.c -lpthread -Wall -Wextra
lionel_screen.o: lionel_screen.c lionel_screen.h lionel_types.h
	gcc -c lionel_screen.c -lpthread -Wall -Wextra
Lionel: lionel_main.o lionel_start.o lionel_program.o lionel_connection.o lionel_configReader.o lionel_memory.o lionel_trama.o lionel_screen.o
	gcc lionel_main.o lionel_start.o lionel_program.o lionel_connection.o lionel_configReader.o lionel_memory.o lionel_trama.o lionel_screen.o -o Lionel -lpthread -Wall -Wextra

clean:
	rm *.o McGruder.exe
	rm *.o Lionel.exe