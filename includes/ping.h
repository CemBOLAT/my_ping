#pragma once

#include "superArray.h"
#include "init.h"
#include "datas.h"
#include "libc.h"

// what should I use for struct for icmp packet

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
void        init_dest_addr(ft_ping *ping);
void        print_ping_banner(ft_ping *ping);
void        update_statistics(ft_ping *ping, double rtt);
bool        is_ipv4(char *str, ft_ping *ping);
bool        is_fdqn(char *str, ft_ping *ping);