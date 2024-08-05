#include "../includes/ping.h"
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <limits.h>

void init_options(Options *options){
    char *op[12] = {"-v", "-?", "-f", "-l", "-n", "-w", "-W", "-p", "-s", "-T", "-t", "--ip-timestamp"};
    int values[12] = {0, 0, 0, 1, 0, 1, 1, 1, 1, 0, 1, 0};

    for (int i = 0; i < 12; i++)
    {
        options[i].option = op[i];
        options[i].value = values[i];
    }
}

void init_round_trip(RoundTrip *round_trip){
    memset(round_trip, 0, sizeof(RoundTrip));
    round_trip->min.tv_sec = LONG_MAX;
    round_trip->min.tv_usec = LONG_MAX;
    round_trip->max.tv_sec = 0;
    round_trip->max.tv_usec = 0;
    round_trip->count = 0;
    round_trip->sum_rtt = 0;
    round_trip->sum_rtt_squared = 0;
}

void init_ping(ft_ping *ping){
    ping->socket = -1;
    ping->arr = createArray();
    ping->hosts = createStringArray();
    bzero(&ping->temp_info, sizeof(struct addrinfo));
    bzero(&ping->dest_addr, sizeof(struct sockaddr_in));
    bzero(&ping->icmp_hdr, sizeof(struct icmp));
    ping->packet = NULL;
    ping->packet_size = 0;
    ping->seq = 0;
    ping->nbr_of_packets = 0;
    init_options(ping->options);
    init_round_trip(&ping->round_trip);
}

int parse_options(int argc, char **argv, ft_ping *ping){
    init_ping(ping);
    bool isOption = false;
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
                    if (ping->options[j].value == 1)
                    {
                        if (i + 1 == argc)
                        {
                            ERROR_MESSAGE(USAGE);
                            ft_perfect_exit(ping);
                        }
                        if (token.type != TokenType_Pattern && !isNumber(argv[i + 1]))
                        {
                            ERROR_MESSAGE(argv[i + 1]);
                            ft_perfect_exit(ping);
                        }
                        token.value = argv[++i];
                        if (token.value == 0)
                        {
                            ERROR_MESSAGE(argv[i]);
                            ft_perfect_exit(ping);
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