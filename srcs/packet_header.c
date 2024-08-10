
#include "../includes/ping.h"
#include <unistd.h>
#include <sys/time.h>

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


void packet_ip_header(ft_ping *ping)
{
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
}

void packet_icmp_header(ft_ping *ping, struct icmphdr *icmp_hdr){
    icmp_hdr->type = ICMP_ECHO;
    icmp_hdr->code = 0;
    icmp_hdr->checksum = 0;
    icmp_hdr->un.echo.id = getpid();
    icmp_hdr->un.echo.sequence = htons(ping->seq);

    packet_type(ping, icmp_hdr);

    my_memcpy(ping->packet, icmp_hdr, sizeof(struct icmphdr));
}

void packet_type(ft_ping *ping, struct icmphdr *icmp_hdr){
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
}