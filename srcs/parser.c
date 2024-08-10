#include "../includes/ping.h"
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <limits.h>
#include <math.h>

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
                if (my_strcmp(argv[i], ping->options[j].option) == 0)
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
                                if (my_strcmp(argv[i + 1], packet_types[k]) == 0)
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
                                if (my_strcmp(argv[i + 1], ip_timestamps[k]) == 0)
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
                            if (my_atoi(argv[i + 1]) > 65535 || my_atoi(argv[i + 1]) < 0)
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