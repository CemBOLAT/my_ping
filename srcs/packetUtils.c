#include "../includes/ping.h"
#include <stdbool.h>
#include <signal.h>
#include <string.h>
#include <unistd.h>
#include <sys/time.h>
#include <netinet/ip_icmp.h>
#include <errno.h>
#include <netinet/ip.h> // iphdr tanımı için gerekli
#include <netinet/in.h> // sockaddr_in tanımı için gerekli
#include <limits.h>

// Fill the packet with the given pattern and length
void fill_pattern(char *packet, const char *pattern, size_t length)
{
    size_t pattern_length = my_strlen(pattern);
    for (size_t i = 0; i < length; ++i)
    {
        packet[i] = pattern[i % pattern_length];
    }
}

void init_dest_addr(ft_ping *ping)
{
    // Initialize destination address
    ping->dest_addr.sin_family = AF_INET;
    // Convert IP address to binary form and store it in dest_addr.sin_addr in network byte order
    if (inet_pton(AF_INET, ping->ip_str, &ping->dest_addr.sin_addr) != 1)
    {
        ERROR_MESSAGE("inet_pton");
        ft_perfect_exit(ping);
    }
}

void print_ping_banner(ft_ping *ping)
{
    int ping_ident = getpid() & 0xFFFF;

    char output[512];
    char print_output[1024];

    sprintf(output, "PING %s (%s)",
           ping->hosts->host[0],
           inet_ntoa(ping->dest_addr.sin_addr)
    );

    if (ping->parametersvalue & TokenType_SendPacketType)
    {
        const char *value = get_option_value(ping->arr, TokenType_SendPacketType);
        if (my_strcmp(value, "timestamp") == 0)
        {
            my_strcat(output, " sending timestamp requests"); // PING google.com (172.217.169.110) 56 data bytes sending timestamp requests
        }
        else if (my_strcmp(value, "address") == 0 || my_strcmp(value, "mask") == 0)
        {
            my_strcat(output, " sending address mask requests");
        }
    }
    else if (ping->parametersvalue & TokenType_Verbose)
    {

        sprintf(print_output, "%s %d data bytes, id 0x%04x = %u",
                output,
                ping->packet_size,
                htons(ping_ident),
                htons(ping_ident));
    }
    else
    {
        sprintf(print_output, "%s %d data bytes",
                output,
                ping->packet_size);
    }
    printf("%s\n", print_output);
    my_bzero(print_output, sizeof(print_output));
    my_bzero(output, sizeof(output));
}

void preloadOption(ft_ping *ping)
{
    if (ping->parametersvalue & TokenType_Preload){
        int preload = my_atoi(get_option_value(ping->arr, TokenType_Preload));
        for (int i = 0; i < preload; i++){
            send_icmp_packet(ping);
            receive_icmp_packet(ping);
        }
    }
}
