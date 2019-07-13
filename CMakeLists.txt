cmake_minimum_required(VERSION 3.12)
project(mcGruder C)

set(CMAKE_C_STANDARD 99)

add_executable(mcGruder mcgruder_main.c mcgruder_start.c mcgruder_start.h mcgruder_screen.c mcgruder_screen.h mcgruder_configReader.c mcgruder_configReader.h mcgruder_types.h mcgruder_connection.c mcgruder_connection.h mcgruder_program.c mcgruder_program.h mcgruder_scanner.c mcgruder_scanner.h lionel_main.c lionel_start.c lionel_start.h lionel_screen.c lionel_screen.h lionel_configReader.c lionel_configReader.h lionel_types.h lionel_memory.c lionel_memory.h lionel_connection.c lionel_connection.h lionel_program.c lionel_program.h mcgruder_memory.c mcgruder_memory.h mcgruder_trama.c mcgruder_trama.h lionel_trama.c lionel_trama.h checksum.c checksum.h)