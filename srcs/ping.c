#include "../includes/ping.h"
#include <stdbool.h>
#include <signal.h>
#include <string.h>
#include <unistd.h>



void send_icmp_packet(ft_ping *ping)
{
    if (sendto(ping->socket, ping->packet, sizeof(ping->packet), 0, (struct sockaddr *)&ping->dest_addr, sizeof(ping->dest_addr)) <= 0)
    {
        ERROR_MESSAGE("sendto");
        ft_perfect_exit(ping);
    }

    printf("ICMP request sent to %s\n", ping->hosts->host[0]);
}

void receive_icmp_packet(ft_ping *ping)
{

}

void init_icmp_packet(ft_ping *ping)
{
    // ICMP header
    memset(&ping->temp_info, 0, sizeof(ping->temp_info));
    ping->temp_info.ai_family = AF_INET;
    ping->temp_info.ai_socktype = SOCK_RAW;
    ping->temp_info.ai_protocol = IPPROTO_ICMP;

    if (getaddrinfo(ping->hosts->host[0], NULL, &ping->temp_info, &ping->info) != 0)
    {
        ERROR_MESSAGE("getaddrinfo");
        ft_perfect_exit(ping);
    }

    ping->dest_addr = *(struct sockaddr_in *)ping->info->ai_addr;
    freeaddrinfo(ping->info);

    // Prepare ICMP header
    memset(&ping->icmp_hdr, 0, sizeof(ping->icmp_hdr));
    ping->icmp_hdr.icmp_type = ICMP_ECHO;
    ping->icmp_hdr.icmp_code = 0;
    ping->icmp_hdr.icmp_id = getpid();
    ping->icmp_hdr.icmp_seq = 1;
    ping->icmp_hdr.icmp_cksum = 0;
    //ping->icmp_hdr.icmp_cksum = checksum(&ping->icmp_hdr, sizeof(ping->icmp_hdr));

    // Copy ICMP header to packet
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
    //signal(SIGINT, ft_perfect_exit);
    while (1)
    {
        send_icmp_packet(ping);
        sleep(1);
        receive_icmp_packet(ping);
    }
}