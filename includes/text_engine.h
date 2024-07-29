
#pragma once

#include <stdio.h>



#define ERROR printf("\033[1;31m\033[0m");
#define RED(str) printf("\033[1;31m%s\033[0m", str)
#define GREEN(str) printf("\033[1;32m%s\033[0m", str)
#define YELLOW(str) printf("\033[1;33m%s\033[0m", str)
#define BLUE(str) printf("\033[1;34m%s\033[0m", str)
#define MAGENTA(str) printf("\033[1;35m%s\033[0m", str)
#define CYAN(str) printf("\033[1;36m%s\033[0m", str)
#define WHITE(str) printf("\033[1;37m%s\033[0m", str)
#define RESET printf("\033[0m");

// void text_engine_test(){
//     RED("RED\n");
//     GREEN("GREEN\n");
//     YELLOW("YELLOW\n");
//     BLUE("BLUE\n");
//     MAGENTA("MAGENTA\n");
//     CYAN("CYAN\n");
//     WHITE("WHITE\n");
//     RESET;
// }


#define ERROR_MESSAGE(str)  printf("\033[1;31mError: %s (File: %s, Line: %d)\n\033[0m", str, __FILE__, __LINE__);
#define USAGE "Usage: ./ft_ping [-?] [-v] [-f] [-l] [-n] [-w] [-W] [-p] [-s] [-T] [-t] [--ip-timestamp] <hostname>\n"