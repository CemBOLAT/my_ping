#include "../includes/ping.h"
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <limits.h>

int parse_options(int argc, char **argv, ft_ping *ping){
    const char  *packet_types[] = {"echo", "address", "mask", "timestamp"}; // Packet types
    const char  *ip_timestamps[] = {"tsonly", "tsaddr"}; // Timestamp options
    bool        isOption = false;

    init_ping(ping); // Initialize ping struct
    for (int i = 1; i < argc; i++)
    {
        isOption = false;
        // Parameter input
        if (argv[i][0] == '-') // Check if the argument is an option
        {
            for (int j = 0; j < 12; j++)
            {
                if (my_strcmp(argv[i], ping->options[j].option) == 0) // Check if the option is valid
                {
                    Token token; // Create a token
                    token.type = j;
                    ping->parametersvalue |= ping->options[j].numericValue; // Set the value of the option
                    isOption = true; // Set isOption to true
                    if (ping->options[j].value != 0) // If the option can have a value
                    {
                        if (i + 1 == argc) // If the value is missing
                        {
                            char output[1024];
                            sprintf(output, "./ping: option requires an argument -- '%s'\n", argv[i]);
                            ERROR_MESSAGE(output);
                            my_bzero(output, 1024);
                            ft_perfect_exit(ping);
                        }
                        if (ping->options[j].value == 4) // If the option is a packet type
                        {
                            for (int k = 0; k < 4; k++) // Check if the packet type is valid
                            {
                                if (my_strcmp(argv[i + 1], packet_types[k]) == 0)
                                {
                                    token.value = argv[i + 1];
                                    break;
                                }
                                if (k == 3) // If the packet type is invalid
                                {
                                    char output[1024];
                                    sprintf(output, "./ping: invalid argument: %s\n", argv[i + 1]);
                                    ERROR_MESSAGE(output);
                                    my_bzero(output, 1024);
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
                                if (k == 1) // If the timestamp option is invalid
                                {
                                    char output[1024];
                                    sprintf(output, "./ping: invalid argument: %s\n", argv[i + 1]);
                                    ERROR_MESSAGE(output);
                                    my_bzero(output, 1024);
                                    ft_perfect_exit(ping);
                                }
                            }
                        }
                        else if (ping->options[j].value == 2) // If the option is a string
                        {
                            token.value = argv[i + 1];
                        }
                        else if (ping->options[j].value == 1 && !isNumber(argv[i + 1])) // If the option is a number but the value is not a number
                        {
                            char output[1024];
                            sprintf(output, "./ping: option value must be a number: %s\n", argv[i + 1]);
                            ERROR_MESSAGE(output);
                            my_bzero(output, 1024);
                            ft_perfect_exit(ping);
                        }
                        else
                        {
                            if (my_atoi(argv[i + 1]) > 65535 || my_atoi(argv[i + 1]) < 0) // If the value is too big (bigger than 65535 is ok for my implementation)
                            {
                                char output[1024];
                                sprintf(output, "./ping: option value too big: %s\n", argv[i + 1]);
                                ERROR_MESSAGE(output);
                                my_bzero(output, 1024);
                                ft_perfect_exit(ping);
                            }
                            token.value = argv[i + 1];
                        }
                    }
                    else
                    {
                        token.value = NULL; // Set the value of the option to NULL
                    }
                    insertArray(ping->arr, token); // Insert the token into the array
                    break;
                }
            }
            if (!isOption) // If the option is not valid
            {
                char output[1024];
                sprintf(output, "./ping: invalid option -- '%s'\n", argv[i]);
                ERROR_MESSAGE(output);
                my_bzero(output, 1024);
                ft_perfect_exit(ping);
            }
        }
        else // if the argument is not an option its a host
        {
            insertStringArray(ping->hosts, argv[i]);   
        }
    }
    return 0;
}