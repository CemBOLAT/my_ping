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

// Helper function to add IP timestamp option
void add_ip_timestamp_option(void *packet, int option_type) {
    struct iphdr *ip_hdr = (struct iphdr *)packet;
    uint8_t *options = (uint8_t *)(packet + sizeof(struct iphdr));
    int option_length = 0;

    // IP timestamp option header
    options[0] = 0x01; // Timestamp option type
    options[1] = 0x01; // Length (will be adjusted below)
    options[2] = option_type; // TSO: Timestamp only (1) or Timestamp + Address (2)
    options[3] = 0x00; // Reserved

    // Adjust option length and header fields
    if (option_type == 1) { // tsonly
        option_length = 8 + 4 * 9; // 8-byte header + 4 bytes per timestamp, up to 9 timestamps
    } else if (option_type == 2) { // tsaddr
        option_length = 8 + 8 * 4; // 8-byte header + 8 bytes per timestamp/address, up to 4 hops
    }

    options[1] = option_length; // Set the option length

    // Initialize the timestamp field
    my_memset(options + 4, 0, option_length - 4); // Fill with zeros

    ip_hdr->ihl = (sizeof(struct iphdr) + option_length) / 4;
    ip_hdr->tot_len = htons(ntohs(ip_hdr->tot_len) + option_length);
}

void packetCycle(ft_ping *ping){
    // Clear packet memory
    my_memset(ping->packet, 0, ping->packet_size + sizeof(struct icmphdr));

    struct iphdr *ip_hdr = (struct iphdr *)ping->packet;
    ip_hdr->ihl = 5; // Header length
    ip_hdr->version = 4; // IPv4
    ip_hdr->tos = 0; // Type of service
    ip_hdr->tot_len = sizeof(struct iphdr) + sizeof(struct icmphdr) + ping->packet_size;
    ip_hdr->id = htons(getpid()); // ID
    ip_hdr->frag_off = 0; // Fragment offset
    ip_hdr->ttl = 64; // Time to live
    ip_hdr->protocol = IPPROTO_ICMP; // Protocol
    ip_hdr->check = 0; // Checksum
    ip_hdr->saddr = 0; // Source address
    ip_hdr->daddr = ping->dest_addr.sin_addr.s_addr; // Destination address

    // Calculate checksum
    ip_hdr->check = checksum((uint16_t *)ip_hdr, sizeof(struct iphdr));

    // Copy IP header to packet

    if (ping->parametersvalue & TokenType_TOS)
    {
        ip_hdr->tos = my_atoi(get_option_value(ping->arr, TokenType_TOS));
    }


    my_memcpy(ping->packet, ip_hdr, sizeof(struct iphdr));

    if (ping->parametersvalue & TokenType_Ip_TimeStamp) {
        const char *ts_option = get_option_value(ping->arr, TokenType_Ip_TimeStamp);
        int option_type = (my_strcmp(ts_option, "tsonly") == 0) ? 1 : 2;
        add_ip_timestamp_option(ping->packet, option_type);
    }

    struct icmphdr *icmp_hdr = (struct icmphdr *)ping->packet;
    icmp_hdr->type = ICMP_ECHO;
    icmp_hdr->code = 0;
    icmp_hdr->checksum = 0;
    icmp_hdr->un.echo.id = getpid();
    icmp_hdr->un.echo.sequence = htons(ping->seq);

    if (ping->parametersvalue & TokenType_SendPacketType)
    {
        const char *packet_type = get_option_value(ping->arr, TokenType_SendPacketType);
        if (my_strcmp(packet_type, "address") == 0)
        {
            icmp_hdr->type = ICMP_ADDRESS; 
        
            uint32_t subnet_mask = htonl(0x00000000); // Replace with the appropriate mask
            my_memcpy(ping->packet + sizeof(struct icmphdr), &subnet_mask, sizeof(subnet_mask));

        }
        else if (my_strcmp(packet_type, "mask") == 0)
        {
            icmp_hdr->type = ICMP_MASKREQ;

            // IP mask option

            uint32_t subnet_mask = htonl(0x00000000); // Replace with the appropriate mask
            my_memcpy(ping->packet + sizeof(struct icmphdr), &subnet_mask, sizeof(subnet_mask));
        }
        else if (my_strcmp(packet_type, "timestamp") == 0)
        {
            icmp_hdr->type = ICMP_TIMESTAMP;
            struct timeval tv;
            gettimeofday(&tv, NULL);
            
            uint32_t originate_timestamp = htonl(tv.tv_sec * 1000 + tv.tv_usec / 1000); // Convert to milliseconds

            // IP timestamp option
            unsigned char ip_opts[12];
            ip_opts[0] = IPOPT_TIMESTAMP;
            ip_opts[1] = 10; // length of the option
            ip_opts[2] = 5;  // pointer to the next free byte
            ip_opts[3] = IPOPT_TS_TSONLY;
            
            my_memcpy(&ip_opts[4], &originate_timestamp, sizeof(originate_timestamp));

            // Copy IP options into packet after ICMP header
            my_memcpy(ping->packet + sizeof(struct icmphdr), ip_opts, sizeof(ip_opts));
        }
    }

    my_memcpy(ping->packet, icmp_hdr, sizeof(struct icmphdr));

    // Add pattern to packet if specified
    const char *pattern = NULL;
    if (ping->parametersvalue & TokenType_Pattern)
    {
        pattern = get_option_value(ping->arr, TokenType_Pattern);
        fill_pattern(ping->packet + sizeof(struct icmp) + sizeof(struct timeval), pattern, ping->packet_size - sizeof(struct icmp) - sizeof(struct timeval));
    }

    // Add timestamp to packet
 

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
            bzero(output, sizeof(output));
        }
    }
}

void init_icmp_packet(ft_ping *ping)
{
    // Parse packet size from options
    ping->packet_size = ping->parametersvalue & TokenType_PacketSize != 0 ? my_atoi(get_option_value(ping->arr, TokenType_PacketSize)) : DEFAULT_PACKET_SIZE;
    
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
    if (ping->parametersvalue & TokenType_Preload){
        int preload = my_atoi(get_option_value(ping->arr, TokenType_Preload));
        for (int i = 0; i < preload; i++){
            send_icmp_packet(ping);
            receive_icmp_packet(ping);
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