#include "../includes/ping.h"
#include <stdio.h>
#include <unistd.h>
#include <sys/time.h>

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
    struct timeval end_time;

    gettimeofday(&end_time, NULL);

    printf("--- %s ping statistics ---\n", global->hosts->host[0]);

    int packets_transmitted = global->nbr_of_packets;
    int packets_received = global->round_trip.count;
    int packet_loss = packets_transmitted - packets_received;
    int packet_loss_percentage = (packet_loss * 100) / packets_transmitted;
    int total_time_ms = (end_time.tv_sec - global->round_trip.time.tv_sec) * 1000 + (end_time.tv_usec - global->round_trip.time.tv_usec) / 1000;

    printf("%d packets transmitted, %d received, %d%% packet loss, time %dms\n",
           packets_transmitted, packets_received, packet_loss_percentage, total_time_ms);

    ft_perfect_exit(global);
}