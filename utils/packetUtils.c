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
#include <math.h>

void fill_pattern(char *packet, const char *pattern, size_t length)
{
    size_t pattern_length = strlen(pattern);
    for (size_t i = 0; i < length; ++i)
    {
        packet[i] = pattern[i % pattern_length];
    }
}

u_short checksum(u_short *buf, int len)
{
    u_short *w = buf;
    int nleft = len;
    int sum = 0;
    u_short temp = 0;

    while (nleft > 1)
    {
        sum += *w++;
        nleft -= 2;
    }

    if (nleft == 1)
    {
        *(u_char *)(&temp) = *(u_char *)w;
        sum += temp;
    }

    sum = (sum >> 16) + (sum & 0xFFFF);
    sum += (sum >> 16);
    return (u_short)(~sum);
}

void add_timestamp(char *packet, size_t offset)
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    memcpy(packet + offset, &tv, sizeof(tv));
}

void init_dest_addr(ft_ping *ping)
{
    // Initialize destination address
    ping->dest_addr.sin_family = AF_INET;
    if (inet_pton(AF_INET, ping->ip_str, &ping->dest_addr.sin_addr) != 1)
    {
        ERROR_MESSAGE("inet_pton");
        ft_perfect_exit(ping);
    }
}

void print_ping_banner(ft_ping *ping)
{
    int ping_ident = getpid() & 0xFFFF;

    char output[1024];

    sprintf(output, "PING %s (%s)",
           ping->hosts->host[0],
           inet_ntoa(ping->dest_addr.sin_addr)
    );

    if (ping->parametersvalue & TokenType_SendPacketType)
    {
        const char *value = get_option_value(ping->arr, TokenType_SendPacketType);
        if (strcmp(value, "timestamp") == 0)
        {
            strcat(output, " sending timestamp requests"); // PING google.com (172.217.169.110) 56 data bytes sending timestamp requests
        }
        else if (strcmp(value, "address") == 0 || strcmp(value, "mask") == 0)
        {
            strcat(output, " sending address mask requests");
        }
    }
    else if (ping->parametersvalue & TokenType_Verbose)
    {

        sprintf(output, "%s %d data bytes, id 0x%04x = %u",
                ping->packet_size,
                output,
                htons(ping_ident),
                htons(ping_ident));
    }
    else
    {
        sprintf(output, "%s %d data bytes",
                output,
                ping->packet_size);
    }
    printf("%s\n", output);
}

void update_statistics(ft_ping *ping, double rtt)
{
    // Update min and max
    if (rtt < ping->round_trip.min)
    {
        ping->round_trip.min = rtt;
    }

    if (rtt > ping->round_trip.max)
    {
        ping->round_trip.max = rtt;
    }

    // Update average
    ping->round_trip.avg = ping->round_trip.sum_rtt / ping->round_trip.count;

    // Update standard deviation

    // Calculate variance
    double variance = (ping->round_trip.sum_rtt_squared / ping->round_trip.count) - (ping->round_trip.avg * ping->round_trip.avg);

    // Calculate standard deviation

    ping->round_trip.std_dev = sqrt(variance);

    // Update sum for average and std deviation
    ping->round_trip.sum_rtt += rtt;
    ping->round_trip.sum_rtt_squared += rtt * rtt;

    // printf("Intermediate sum rtt = %.3f ms\n", ping->round_trip.sum_rtt);
    // printf("Intermediate sum rtt squared = %.3f ms\n", ping->round_trip.sum_rtt_squared);

    ping->round_trip.count++;
}