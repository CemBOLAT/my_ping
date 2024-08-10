#pragma once

#include "parser.h"
#include "libc.h"
#include "datas.h"
#include "calc.h"
#include "init.h"
// what should I use for struct for icmp packet

extern ft_ping *global;

void        ft_perfect_exit(ft_ping *ping);
void        signal_exit(int signum);

void        execute_ping(ft_ping *ping);
void        receive_icmp_packet(ft_ping *ping);
void        send_icmp_packet(ft_ping *ping);
void        packetCycle(ft_ping *ping);
void        packet_icmp_header(ft_ping *ping, struct icmphdr *icmp_hdr);
void        packet_type(ft_ping *ping, struct icmphdr *icmp_hdr);

void        fill_pattern(char *packet, const char *pattern, size_t length);
void        preloadOption(ft_ping *ping);
void        print_ping_banner(ft_ping *ping);
void        add_ip_timestamp_option(void *packet, int option_type);
void        packet_ip_header(ft_ping *ping);