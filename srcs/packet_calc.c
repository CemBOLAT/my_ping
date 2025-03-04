
#include "../includes/ping.h"
#include <math.h>

// Calculate checksum
// https://tools.ietf.org/html/rfc1071
// https://datatracker.ietf.org/doc/html/rfc1071
u_short checksum(u_short *buf, int len)
{
    u_short *w = buf;
    int     nleft = len; // Length of the buffer (in bytes)
    int     sum = 0;    // 32-bit sum
    u_short temp = 0; // 16-bit temp

    while (nleft > 1)
    {
        sum += *w++;
        nleft -= 2;
    }

    if (nleft == 1) // one byte left
    {
        *(u_char *)(&temp) = *(u_char *)w;
        sum += temp;
    }

    sum = (sum >> 16) + (sum & 0xFFFF); // sum = sums of high and low 16 bits
    sum += (sum >> 16); // add carry
    return (u_short)(~sum); // one's complement
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
    ping->round_trip.count++;
}
