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
    my_memset(ping->packet, 0, ping->packet_size + sizeof(struct icmphdr));

    packet_ip_header(ping);

    if (ping->parametersvalue & TokenType_Ip_TimeStamp) {
        const char *ts_option = get_option_value(ping->arr, TokenType_Ip_TimeStamp);
        int option_type = (my_strcmp(ts_option, "tsonly") == 0) ? 1 : 2;
        add_ip_timestamp_option(ping->packet, option_type);
    }

    struct icmphdr *icmp_hdr = (struct icmphdr *)ping->packet;
    packet_icmp_header(ping, icmp_hdr);

    // Add pattern to packet if specified
    const char *pattern = NULL;
    if (ping->parametersvalue & TokenType_Pattern)
    {
        pattern = get_option_value(ping->arr, TokenType_Pattern);
        fill_pattern(ping->packet + sizeof(struct icmp) + sizeof(struct timeval), pattern, ping->packet_size - sizeof(struct icmp) - sizeof(struct timeval));
    } 

    // Calculate checksum
    icmp_hdr->checksum = checksum((uint16_t *)ping->packet, ping->packet_size);

    // Copy ICMP header to packet

    my_memcpy(ping->packet, icmp_hdr, sizeof(struct icmp));
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
    ping->is_received = false;
    //printf("ICMP request sent to %s\n", ping->hosts->host[0]);
}

void receive_icmp_packet(ft_ping *ping) {
    char buffer[1024];
    struct sockaddr_in recv_addr;
    socklen_t addr_len = sizeof(recv_addr);
    int bytes_received = recvfrom(ping->socket, buffer, sizeof(buffer), 0, (struct sockaddr *)&recv_addr, &addr_len);

    bool isFlooding = ping->parametersvalue & TokenType_Flood != 0;
    
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

            ping->is_received = true;

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
            my_bzero(output, sizeof(output));
        }
    }
}

void execute_ping(ft_ping *ping){
    // if we have -? option we should print help and exit because -? is most important option
    if (ping->parametersvalue & TokenType_Help){
        YELLOW(HELP);
        ft_perfect_exit(ping);
    }
    // firstly if we dont have any host to ping we should exit
    if (ping->hosts->size == 0){
        ERROR_MESSAGE("ping: missing host operand");
        YELLOW(USAGE);
        ft_perfect_exit(ping);
    }

    if (is_ipv4(ping->hosts->host[0], ping) == false && is_fdqn(ping->hosts->host[0], ping) == false){
        ERROR_MESSAGE("./ft_ping: unknown host");
        ft_perfect_exit(ping);
    }

    init_socket(ping);
    init_icmp_packet(ping);
    signal(SIGINT, signal_exit);
    print_ping_banner(ping);
    gettimeofday(&ping->round_trip.time, NULL);
    preloadOption(ping);

    struct timeval end;
    size_t deadlineValue = ((ping->parametersvalue & TokenType_Deadline) != 0) ? my_atoi(get_option_value(ping->arr, TokenType_Deadline)) : INT_MAX;
    size_t timeoutValue = ((ping->parametersvalue & TokenType_Timeout) != 0) ? my_atoi(get_option_value(ping->arr, TokenType_Timeout)) : INT_MAX;

    while (1)
    {
        send_icmp_packet(ping);
        if (ping->parametersvalue & TokenType_Deadline){
            gettimeofday(&end, NULL);
            if (end.tv_sec - ping->round_trip.time.tv_sec >= deadlineValue){
                signal_exit(SIGINT);
            }
        }
        if (ping->parametersvalue & TokenType_Timeout){
            while (1){
                gettimeofday(&end, NULL);
                if (end.tv_sec - ping->round_trip.sendTime.tv_sec >= timeoutValue)
                    break;
                if (ping->is_received)
                    break;
                receive_icmp_packet(ping);
            }
        }
        else {
            receive_icmp_packet(ping);
        }
        if (ping->parametersvalue & TokenType_Flood){
            if (ping->seq % 400 == 0)
            {
                printf(".....");
            }
        }
        else
            sleep(1); // okey
    }
}