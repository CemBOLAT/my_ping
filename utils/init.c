#include "../includes/ping.h"
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <limits.h>
#include <math.h>

void init_options(Options *options){
    char *op[12] = {"-v", "-?", "-f", "-l", "-n", "-w", "-W", "-p", "-s", "-T", "-t", "--ip-timestamp"};
    int values[12] = {0,    0,    0,    1,    0,    1,    1,    2,    1,    1,    4,           3};

    for (int i = 0; i < 12; i++)
    {
        options[i].option = op[i];
        options[i].value = values[i];
        options[i].numericValue = pow(2, i);
    }
}

void init_round_trip(ft_ping *ping){
    my_memset(&ping->round_trip, 0, sizeof(RoundTrip));
    ping->round_trip.min = INT_MAX;
    ping->round_trip.avg = 0.0;
    ping->round_trip.max = INT_MIN;
    ping->round_trip.std_dev = 0.0;

    ping->round_trip.sum_rtt = 0.0;
    ping->round_trip.sum_rtt_squared = 0.0;
    ping->round_trip.count = 0;
    
}

void init_ping(ft_ping *ping){
    ping->socket = -1;
    ping->arr = createArray();
    ping->hosts = createStringArray();
    bzero(&ping->dest_addr, sizeof(struct sockaddr_in));
    bzero(&ping->options, sizeof(Options) * 12);
    ping->packet = NULL;
    ping->packet_size = 0;
    ping->seq = 0;
    ping->nbr_of_packets = 0;
    ping->parametersvalue = 0;
    init_options(ping->options);
    init_round_trip(ping);
}
