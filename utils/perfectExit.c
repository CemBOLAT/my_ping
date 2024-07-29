#include "../includes/superArray.h"
#include <stdio.h>

void ft_perfect_exit(ft_ping *ping){
    if (!ping){
        ERROR_MESSAGE("ft_ping is NULL");
        exit(1);
    }
    if (ping->arr){
        freeArray(ping->arr);
    }
    if (ping->hosts){
        freeStringArray(ping->hosts);
    }
    exit(0);
}