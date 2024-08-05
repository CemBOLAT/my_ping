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


void packetCycle(ft_ping *ping){
    // Clear packet memory
    memset(ping->packet, 0, ping->packet_size);

    // Initialize ICMP header

    struct icmp *icmp_hdr = (struct icmp *)ping->packet;
    icmp_hdr->icmp_type = ICMP_ECHO;
    icmp_hdr->icmp_code = 0;
    icmp_hdr->icmp_id = getpid();
    icmp_hdr->icmp_seq = ping->seq++;
    icmp_hdr->icmp_cksum = 0;

    // Copy ICMP header to packet
    memcpy(ping->packet, icmp_hdr, sizeof(struct icmp));

    // Add pattern to packet if specified
    const char *pattern = NULL;
    if (have_option(ping->arr, TokenType_Pattern))
    {
        pattern = get_option_value(ping->arr, TokenType_Pattern);
        fill_pattern(ping->packet + sizeof(struct icmp) + sizeof(struct timeval), pattern, ping->packet_size - sizeof(struct icmp) - sizeof(struct timeval));
    }

    // Add timestamp to packet
    if (have_option(ping->arr, TokenType_TimeStamp))
    {
        add_timestamp(ping->packet, sizeof(struct icmp) + (pattern == NULL ? 0 : strlen(pattern)));
    }

    // Calculate checksum
    icmp_hdr->icmp_cksum = checksum((uint16_t *)ping->packet, ping->packet_size);

    // Copy ICMP header to packet

    memcpy(ping->packet, icmp_hdr, sizeof(struct icmp));
}

void send_icmp_packet(ft_ping *ping)
{
    packetCycle(ping);
    gettimeofday(&ping->round_trip.sendTime, NULL);
    if (sendto(ping->socket, ping->packet, ping->packet_size, 0, (struct sockaddr *)&ping->dest_addr, sizeof(ping->dest_addr)) <= 0)
    {
        ERROR_MESSAGE("sendto");
        // use errno to determine the cause of the error
        //printf("errno: %d\n", errno);
        ft_perfect_exit(ping);
    }
    ping->nbr_of_packets++;
    //printf("ICMP request sent to %s\n", ping->hosts->host[0]);
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

void receive_icmp_packet(ft_ping *ping) {
    char buffer[1024];
    struct sockaddr_in recv_addr;
    socklen_t addr_len = sizeof(recv_addr);
    int bytes_received = recvfrom(ping->socket, buffer, sizeof(buffer), 0, (struct sockaddr *)&recv_addr, &addr_len);
    
    if (bytes_received < 0) {
        if (errno == EAGAIN || errno == EWOULDBLOCK) {
            RED("Request timed out.\n");
            RESET;
        } else {
            ERROR_MESSAGE("recvfrom");
            ft_perfect_exit(ping);
        }
    } else {
        struct iphdr *ip_hdr = (struct iphdr *)buffer;
        int ip_header_len = ip_hdr->ihl * 4;
        struct icmphdr *icmp_hdr = (struct icmphdr *)(buffer + ip_header_len);
        
        if (icmp_hdr->type == ICMP_ECHOREPLY && icmp_hdr->un.echo.id == getpid()) {
            gettimeofday(&ping->round_trip.recvTime, NULL);


            double send_time = ping->round_trip.sendTime.tv_sec + (double)ping->round_trip.sendTime.tv_usec / 1000000;
            double recv_time = ping->round_trip.recvTime.tv_sec + (double)ping->round_trip.recvTime.tv_usec / 1000000;

            double rtt = (recv_time - send_time) * 1000;


            printf("%ld bytes from %s: icmp_seq=%d ttl=%d time=%.1f ms\n",
                   bytes_received - ip_header_len + sizeof(struct icmphdr),
                   inet_ntoa(recv_addr.sin_addr),
                   icmp_hdr->un.echo.sequence,
                   ip_hdr->ttl,
                   rtt);
            update_statistics(ping, rtt);
        } else {
            char output[1024];
            sprintf(output, "Cannot handle ICMP packet with type %d", icmp_hdr->type);
            ERROR_MESSAGE(output);
            RESET;
            bzero(output, sizeof(output));
        }
    }
}

void init_icmp_packet(ft_ping *ping)
{
    // Parse packet size from options
    ping->packet_size = have_option(ping->arr, TokenType_PacketSize) ? atoi(get_option_value(ping->arr, TokenType_PacketSize)) : DEFAULT_PACKET_SIZE;
    if (ping->packet_size < sizeof(struct icmp) + sizeof(struct timeval))
    {
        ERROR_MESSAGE("ping: packet size too small");
        ft_perfect_exit(ping);
    }

    // Allocate memory for packet
    ping->packet = (char *)malloc(ping->packet_size);
    if (ping->packet == NULL)
    {
        ERROR_MESSAGE("malloc");
        ft_perfect_exit(ping);
    }

    packetCycle(ping);
}

void init_dest_addr(ft_ping *ping)
{
    // Initialize destination address
    ping->dest_addr.sin_family = AF_INET;
    ping->dest_addr.sin_port = 0;
    ping->dest_addr.sin_addr.s_addr = inet_addr(ping->hosts->host[0]);
}

void print_ping_banner(ft_ping *ping)
{
    printf("PING %s (%s) %d bytes of data.\n",
           ping->hosts->host[0],
           inet_ntoa(ping->dest_addr.sin_addr),
           ping->packet_size);
}

void execute_ping(ft_ping *ping){
    // firstly if we dont have any host to ping we should exit
    if (ping->hosts->size == 0){
        ERROR_MESSAGE("ping: missing host operand");
        YELLOW(USAGE);
        ft_perfect_exit(ping);
    }
    // if we have -? option we should print help and exit because -? is most important option

    if (have_option(ping->arr, TokenType_Help)){
        YELLOW(HELP);
        ft_perfect_exit(ping);
    }
    

    init_socket(ping);
    init_icmp_packet(ping);
    init_dest_addr(ping);
    signal(SIGINT, signal_exit);
    print_ping_banner(ping);
    gettimeofday(&ping->round_trip.time, NULL);
    while (1)
    {
        send_icmp_packet(ping);
        receive_icmp_packet(ping);
        sleep(1); // okey
    }
}