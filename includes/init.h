#pragma once

#include "datas.h"

void init_options(Options *options);

void init_round_trip(ft_ping *ping);

void init_ping(ft_ping *ping);

void init_dest_addr(ft_ping *ping);

void init_socket(ft_ping *ping);
void init_icmp_packet(ft_ping *ping);