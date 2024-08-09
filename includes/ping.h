#pragma once

#include "superArray.h"

// what should I use for struct for icmp packet

#include <stdint.h>
#include <sys/socket.h>
#include <netinet/ip_icmp.h>
#include <netinet/ip.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdbool.h>
#include <netinet/ip.h> // iphdr tanımı için gerekli
#include <netinet/in.h> // sockaddr_in tanımı için gerekli
#include <netinet/ip_icmp.h>
#include <netdb.h>

typedef struct s_round_trip {
    double min;
    double max;
    double avg;
    double std_dev;

    struct timeval sendTime;
    struct timeval recvTime;

    double sum_rtt;  // To calculate average
    double sum_rtt_squared;  // To calculate standard deviation
    int count;  // Number of RTT measurements

    struct timeval time;
} RoundTrip;

typedef struct s_ping {
    TokenArray          *arr;
    int                 socket;
    HostArray           *hosts;
    Options             options[12];
    RoundTrip           round_trip;
    struct addrinfo      temp_info;
    struct addrinfo      *info;
    struct sockaddr_in  dest_addr;
    struct icmphdr      icmp_hdr;
    struct icmp         icmp_packet;
    char                *packet;
    int                 packet_size;
    int                 seq;

    size_t              nbr_of_packets;
    char                ip_str[INET_ADDRSTRLEN];
} ft_ping;

extern ft_ping *global;

int         parse_options(int argc, char **argv, ft_ping *ping);

void        ft_perfect_exit(ft_ping *ping);
void        execute_ping(ft_ping *ping);

void        init_socket(ft_ping *ping);

bool        have_option(TokenArray *arr, TokenType type);
TokenType   get_option(TokenArray *arr, TokenType type);
bool        isNumber(char *str);
const char  *get_option_value(TokenArray *arr, TokenType type);
void        signal_exit(int signum);
void        fill_pattern(char *packet, const char *pattern, size_t length);
u_short     checksum(u_short *buf, int len);
void        add_timestamp(char *packet, size_t offset);
void        init_dest_addr(ft_ping *ping);
void        print_ping_banner(ft_ping *ping);
void        update_statistics(ft_ping *ping, double rtt);
bool        is_ipv4(char *str, ft_ping *ping);
bool is_fdqn(char *str, ft_ping *ping);