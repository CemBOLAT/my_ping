#include "./includes/ping.h"
#include <stdio.h>

ft_ping *global;
int main(int argc, char **argv){

    ft_ping ping;

    global = &ping;

    if (argc == 1){
        ERROR_MESSAGE("ping: missing host operand");
        YELLOW(USAGE);
        return 1;
    }

    if (parse_options(argc, argv, &ping) == 1){
        ft_perfect_exit(&ping);
    }

    execute_ping(&ping);
}