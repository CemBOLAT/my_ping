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
#include <netinet/ip_icmp.h>

void packetCycle(ft_ping *ping){
    // Clear packet memory
    memset(ping->packet, 0, ping->packet_size + sizeof(struct icmphdr));

    // Initialize ICMP header

    // struct icmp *icmp_hdr = (struct icmp *)ping->packet;
    // icmp_hdr->icmp_type = ICMP_ECHO;
    // icmp_hdr->icmp_code = 0;
    // icmp_hdr->icmp_id = getpid();
    // icmp_hdr->icmp_seq = ping->seq++;
    // icmp_hdr->icmp_cksum = 0;

    // // Copy ICMP header to packet
    // memcpy(ping->packet, icmp_hdr, sizeof(struct icmp));

    struct icmphdr *icmp_hdr = (struct icmphdr *)ping->packet;
    icmp_hdr->type = ICMP_ECHO;
    icmp_hdr->code = 0;
    icmp_hdr->checksum = 0;
    icmp_hdr->un.echo.id = getpid();
    icmp_hdr->un.echo.sequence = htons(ping->seq);

    memcpy(ping->packet, icmp_hdr, sizeof(struct icmphdr));

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
    icmp_hdr->checksum = checksum((uint16_t *)ping->packet, ping->packet_size);

    // Copy ICMP header to packet

    memcpy(ping->packet, icmp_hdr, sizeof(struct icmp));
}

void send_icmp_packet(ft_ping *ping)
{
    packetCycle(ping);
    gettimeofday(&ping->round_trip.sendTime, NULL);
    if (sendto(ping->socket, ping->packet, ping->packet_size + sizeof(struct icmphdr), 0, (struct sockaddr *)&ping->dest_addr, sizeof(ping->dest_addr)) <= 0)
    {
        ERROR_MESSAGE("sendto");
        // use errno to determine the cause of the error
        //printf("errno: %d\n", errno);
        ft_perfect_exit(ping);
    }
    ping->nbr_of_packets++;
    ping->seq++;
    //printf("ICMP request sent to %s\n", ping->hosts->host[0]);
}

void receive_icmp_packet(ft_ping *ping) {
    char buffer[1024];
    struct sockaddr_in recv_addr;
    socklen_t addr_len = sizeof(recv_addr);
    int bytes_received = recvfrom(ping->socket, buffer, sizeof(buffer), 0, (struct sockaddr *)&recv_addr, &addr_len);

    bool isFlooding = have_option(ping->arr, TokenType_Flood);
    
    if (bytes_received < 0) {
        if (errno == EAGAIN || errno == EWOULDBLOCK) {
            // RED("Request timed out.\n");
            // RESET;
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


            if (isFlooding == false){
                printf("%ld bytes from %s: icmp_seq=%lu ttl=%d time=%.3f ms\n",
                                bytes_received - ip_header_len,
                                inet_ntoa(recv_addr.sin_addr),
                                ntohs(icmp_hdr->un.echo.sequence),
                                ip_hdr->ttl,
                                rtt);
            }
            
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
    
    if (ping->packet_size > 65399)
    {
        char output[1024];
        sprintf(output, "./ft_ping: option value too big: %d", ping->packet_size);
        ERROR_MESSAGE(output);
        ft_perfect_exit(ping);
    }

    // Allocate memory for packet
    ping->packet = (char *)malloc(ping->packet_size + sizeof(struct icmphdr));
    if (ping->packet == NULL)
    {
        ERROR_MESSAGE("malloc");
        ft_perfect_exit(ping);
    }

    packetCycle(ping);
}

void preloadOption(ft_ping *ping)
{
    if (have_option(ping->arr, TokenType_Preload)){
        int preload = atoi(get_option_value(ping->arr, TokenType_Preload));
        for (int i = 0; i < preload; i++){
            send_icmp_packet(ping);
            receive_icmp_packet(ping);
        }
    }
}

void floodOption(ft_ping *ping)
{
    int count = 0;
    if (have_option(ping->arr, TokenType_Flood)){
        while (1){
            send_icmp_packet(ping);
            receive_icmp_packet(ping);
            count++;
            if (count % 400 == 0){
                printf(".....");
            }
        }
    }
}


void execute_ping(ft_ping *ping){
    // if we have -? option we should print help and exit because -? is most important option
    if (have_option(ping->arr, TokenType_Help)){
        YELLOW(HELP);
        ft_perfect_exit(ping);
    }
    // firstly if we dont have any host to ping we should exit
    if (ping->hosts->size == 0){
        ERROR_MESSAGE("ping: missing host operand");
        YELLOW(USAGE);
        ft_perfect_exit(ping);
    }

    if (is_ipv4(ping->hosts->host[0]) == false){
        ERROR_MESSAGE("./ft_ping: unknown host");
        ft_perfect_exit(ping);
    }

    init_socket(ping);
    init_icmp_packet(ping);
    signal(SIGINT, signal_exit);
    print_ping_banner(ping);
    gettimeofday(&ping->round_trip.time, NULL);
    preloadOption(ping);
    floodOption(ping);
    while (1)
    {
        send_icmp_packet(ping);
        receive_icmp_packet(ping);
        sleep(1); // okey
    }
}