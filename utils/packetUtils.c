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
    if (inet_pton(AF_INET, ping->hosts->host[0], &ping->dest_addr.sin_addr) != 1)
    {
        ERROR_MESSAGE("inet_pton");
        ft_perfect_exit(ping);
    }
}

void print_ping_banner(ft_ping *ping)
{
    int ping_ident = getpid() & 0xFFFF;
    if (have_option(ping->arr, TokenType_Verbose))
    {
        printf("PING %s (%s) %d data bytes, id 0x%04x = %u\n",
            ping->hosts->host[0],
            inet_ntoa(ping->dest_addr.sin_addr),
            ping->packet_size, // dent & 0xFFFF
            htons(ping_ident), // htons(ping_ident)
            htons(ping_ident));
    }
    else
    {
        printf("PING %s (%s) %d data bytes\n",
           ping->hosts->host[0],
           inet_ntoa(ping->dest_addr.sin_addr),
           ping->packet_size);
    }
}

void update_statistics(ft_ping *ping, double rtt)
{
    // Update min
    double min_rtt = (ping->round_trip.min.tv_sec * 1000.0) + (ping->round_trip.min.tv_usec / 1000.0);
    if (rtt < min_rtt || ping->round_trip.count == 0)
    {
        ping->round_trip.min.tv_sec = (long)(rtt / 1000.0);
        ping->round_trip.min.tv_usec = (long)((rtt - ping->round_trip.min.tv_sec * 1000.0) * 1000.0);
    }

    // Update max
    double max_rtt = (ping->round_trip.max.tv_sec * 1000.0) + (ping->round_trip.max.tv_usec / 1000.0);
    if (rtt > max_rtt || ping->round_trip.count == 0)
    {
        ping->round_trip.max.tv_sec = (long)(rtt / 1000.0);
        ping->round_trip.max.tv_usec = (long)((rtt - ping->round_trip.max.tv_sec * 1000.0) * 1000.0);
    }

    // Update sum for average and std deviation
    ping->round_trip.sum_rtt += rtt;
    ping->round_trip.sum_rtt_squared += rtt * rtt;
    ping->round_trip.count++;
}