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

void send_icmp_packet(ft_ping *ping)
{
    if (sendto(ping->socket, ping->packet, ping->packet_size, 0, (struct sockaddr *)&ping->dest_addr, sizeof(ping->dest_addr)) <= 0)
    {
        ERROR_MESSAGE("sendto");
        ft_perfect_exit(ping);
    }

    printf("ICMP request sent to %s\n", ping->hosts->host[0]);
}

void receive_icmp_packet(ft_ping *ping)
{
    char buffer[1024];
    struct sockaddr_in recv_addr;
    socklen_t addr_len = sizeof(recv_addr);
    int bytes_received = recvfrom(ping->socket, buffer, sizeof(buffer), 0, (struct sockaddr *)&recv_addr, &addr_len);
    if (bytes_received < 0)
    {
        if (errno == EAGAIN || errno == EWOULDBLOCK)
        {
            RED("Request timed out.\n");
            RESET;
        }
        else
        {
            ERROR_MESSAGE("recvfrom");
            ft_perfect_exit(ping);
        }
    }
    else
    {
        // Process ICMP reply
        struct iphdr *ip_hdr = (struct iphdr *)buffer;
        int ip_header_len = ip_hdr->ihl * 4;

        struct icmphdr *icmp_hdr = (struct icmphdr *)(buffer + ip_header_len);
        
        if (icmp_hdr->type == ICMP_ECHOREPLY)
        {
            if (icmp_hdr->un.echo.id == getpid())
            {
                struct timeval *sent_time = (struct timeval *)(buffer + ip_header_len + sizeof(struct icmphdr));
                struct timeval recv_time;
                gettimeofday(&recv_time, NULL);
                
                long rtt = (recv_time.tv_sec - sent_time->tv_sec) * 1000.0 + (recv_time.tv_usec - sent_time->tv_usec) / 1000.0;

                printf("ICMP reply from %s: seq=%u ttl=%d time=%ld ms\n",
                       inet_ntoa(recv_addr.sin_addr),
                       ntohs(icmp_hdr->un.echo.sequence),
                       ip_hdr->ttl,
                       rtt);
            }
            else
            {
                printf("Received ICMP packet with unmatched id: %d\n", icmp_hdr->un.echo.id);
            }
        }
        else
        {
            printf("Received non-echo reply ICMP packet: type=%d, code=%d\n", icmp_hdr->type, icmp_hdr->code);
        }
    }
}


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

    // Clear packet memory
    memset(ping->packet, 0, ping->packet_size);

    // Initialize ICMP header
    ping->icmp_hdr.icmp_type = ICMP_ECHO;
    ping->icmp_hdr.icmp_code = 0;
    ping->icmp_hdr.icmp_id = getpid();
    ping->icmp_hdr.icmp_seq = 0;
    ping->icmp_hdr.icmp_cksum = 0;

    // Copy ICMP header to packet
    memcpy(ping->packet, &ping->icmp_hdr, sizeof(ping->icmp_hdr));

    // Add pattern to packet if specified
    const char *pattern = NULL;
    if (have_option(ping->arr, TokenType_Pattern))
    {
        pattern = get_option_value(ping->arr, TokenType_Pattern);
        fill_pattern(ping->packet + sizeof(ping->icmp_hdr) + sizeof(struct timeval), pattern, ping->packet_size - sizeof(ping->icmp_hdr) - sizeof(struct timeval));
    }

    // Add timestamp to packet
    if (have_option(ping->arr, TokenType_TimeStamp))
    {
        add_timestamp(ping->packet, sizeof(ping->icmp_hdr) + (pattern == NULL ? 0 : strlen(pattern)));
    }

    // Calculate checksum
    ping->icmp_hdr.icmp_cksum = checksum((uint16_t *)ping->packet, ping->packet_size);
    memcpy(ping->packet, &ping->icmp_hdr, sizeof(ping->icmp_hdr));
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
    signal(SIGINT, signal_exit);
    while (1)
    {
        send_icmp_packet(ping);
        receive_icmp_packet(ping);
    }
}