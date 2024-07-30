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
#include <netinet/ip_icmp.h>
#include <netdb.h>

typedef struct s_ping {
    TokenArray          *arr;
    int                 socket;
    HostArray           *hosts;
    Options             options[12];
    struct addrinfo      temp_info;
    struct addrinfo      *info;
    struct sockaddr_in  dest_addr;
    struct icmp         icmp_hdr;
    char                *packet;
    int                 packet_size;
} ft_ping;

ft_ping *global;

int         parse_options(int argc, char **argv, ft_ping *ping);

void        ft_perfect_exit(ft_ping *ping);
void        execute_ping(ft_ping *ping);

void        init_socket(ft_ping *ping);

bool        have_option(TokenArray *arr, TokenType type);
TokenType   get_option(TokenArray *arr, TokenType type);
bool        isNumber(char *str);
const char  *get_option_value(TokenArray *arr, TokenType type);
void        signal_exit(int signum);