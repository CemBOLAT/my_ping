#include "../includes/ping.h"
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <limits.h>
#include <math.h>

void init_options(Options *options){
    char *op[12] = {"-v", "-?", "-f", "-l", "-n", "-w", "-W", "-p", "-s", "-T", "-t", "--ip-timestamp"};
    int values[12] = {0,    0,    0,    1,    0,    1,    1,    2,    1,    0,    4,           3};

    for (int i = 0; i < 12; i++)
    {
        options[i].option = op[i];
        options[i].value = values[i];
        options[i].numericValue = pow(2, i);
    }
}

void init_round_trip(ft_ping *ping){
    memset(&ping->round_trip, 0, sizeof(RoundTrip));
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
    bzero(&ping->temp_info, sizeof(struct addrinfo));
    bzero(&ping->dest_addr, sizeof(struct sockaddr_in));
    bzero(&ping->icmp_hdr, sizeof(struct icmp));
    bzero(&ping->options, sizeof(Options) * 12);
    ping->packet = NULL;
    ping->packet_size = 0;
    ping->seq = 0;
    ping->nbr_of_packets = 0;
    ping->parametersvalue = 0;
    init_options(ping->options);
    init_round_trip(ping);
}

int parse_options(int argc, char **argv, ft_ping *ping){
    init_ping(ping);
    bool isOption = false;
    char *ip_timestamps[] = {"tsonly", "tsaddr"};
    char *packet_types[] = {"echo", "address", "mask", "timestamp"};
    for (int i = 1; i < argc; i++)
    {
        isOption = false;
        // Parameter input
        if (argv[i][0] == '-')
        {
            for (int j = 0; j < 12; j++)
            {
                if (strcmp(argv[i], ping->options[j].option) == 0)
                {
                    isOption = true;
                    Token token;
                    token.type = j;
                    ping->parametersvalue |= ping->options[j].numericValue;
                    if (ping->options[j].value != 0)
                    {
                        if (i + 1 == argc)
                        {
                            ERROR_MESSAGE(USAGE);
                            ft_perfect_exit(ping);
                        }
                        if (ping->options[j].value == 4)
                        {
                            for (int k = 0; k < 4; k++)
                            {
                                if (strcmp(argv[i + 1], packet_types[k]) == 0)
                                {
                                    token.value = argv[i + 1];
                                    break;
                                }
                                if (k == 3)
                                {
                                    ERROR_MESSAGE(argv[i + 1]);
                                    ft_perfect_exit(ping);
                                }
                            }
                        }
                        else
                        if (ping->options[j].value == 3)
                        {
                            for (int k = 0; k < 2; k++)
                            {
                                if (strcmp(argv[i + 1], ip_timestamps[k]) == 0)
                                {
                                    token.value = argv[i + 1];
                                    break;
                                }
                                if (k == 1)
                                {
                                    ERROR_MESSAGE(argv[i + 1]);
                                    ft_perfect_exit(ping);
                                }
                            }
                        }
                        else if (ping->options[j].value == 2)
                        {
                            token.value = argv[i + 1];
                            if (token.value == 0)
                            {
                                ERROR_MESSAGE(argv[i + 1]);
                                ft_perfect_exit(ping);
                            }
                        }
                        else if (ping->options[j].value == 1 && !isNumber(argv[i + 1]))
                        {
                            ERROR_MESSAGE(argv[i + 1]);
                            ft_perfect_exit(ping);
                        }
                        else
                        {
                            if (atoi(argv[i + 1]) > 65535 || atoi(argv[i + 1]) < 0)
                            {
                                printf("./ping: option value too big: %s\n", argv[i + 1]);
                                ft_perfect_exit(ping);
                            }
                            token.value = argv[i + 1];
                        }
                    }
                    else
                    {
                        token.value = NULL;
                    }
                    insertArray(ping->arr, token);
                    break;
                }
            }
            if (!isOption)
            {
                ERROR_MESSAGE(argv[i]);
                ft_perfect_exit(ping);
            }
        }
        else
        {
            insertStringArray(ping->hosts, argv[i]);   
        }
    }
    return 0;
}