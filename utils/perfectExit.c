#include "../includes/ping.h"
#include <stdio.h>
#include <unistd.h>

void ft_perfect_exit(ft_ping *ping){
    if (!ping){
        ERROR_MESSAGE("ft_ping is NULL");
        exit(1);
    }
    // if (ping->arr){
    //     freeArray(ping->arr);
    // }
    // if (ping->hosts){
    //     freeStringArray(ping->hosts);
    // }
    // if (ping->socket != -1){
    //     close(ping->socket);
    // }

    // if (ping->packet){
    //     free(ping->packet);
    // }
    exit(0);
}

void signal_exit(int signum){
    ft_perfect_exit(global);
}