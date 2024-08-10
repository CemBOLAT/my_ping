#pragma once

#include "datas.h"

void        update_statistics(ft_ping *ping, double rtt);
u_short     checksum(u_short *buf, int len);
bool        isNumber(char *str);
bool        is_ipv4(char *str, ft_ping *ping);
bool        is_fdqn(char *str, ft_ping *ping);