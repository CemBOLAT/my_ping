#include "../includes/ping.h"
#include <stdio.h>
#include <unistd.h>
#include <sys/time.h>

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
    if (ping->socket != -1){
        close(ping->socket);
    }

    if (ping->packet){
        free(ping->packet);
    }
    exit(0);
}

void signal_exit(int signum){

    printf("\r--- %s ping statistics ---\n", global->hosts->host[0]);

    int packets_transmitted = global->nbr_of_packets;
    int packets_received = global->round_trip.count;
    int packet_loss = packets_transmitted - packets_received;
    int packet_loss_percentage = (packet_loss * 100) / packets_transmitted;

    printf("%d packets transmitted, %d received, %d%% packet loss\n",
           packets_transmitted, packets_received, packet_loss_percentage);

    if (packets_received > 0)
    {
        printf("round-trip min/avg/max/stddev = %.3f/%.3f/%.3f/%.3f ms\n",
                global->round_trip.min, global->round_trip.avg, global->round_trip.max, global->round_trip.std_dev);
    }

    ft_perfect_exit(global);
}