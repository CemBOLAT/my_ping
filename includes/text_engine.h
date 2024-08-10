
#pragma once

#include <stdio.h>



#define ERROR printf("\033[1;31m\033[0m");
#define RED(str) printf("\033[1;31m%s\033[0m", str)
#define GREEN(str) printf("\033[1;32m%s\033[0m", str)
#define YELLOW(str) printf("\033[1;33m%s\033[0m", str)
#define BLUE(str) printf("\033[1;34m%s\033[0m", str)
#define MAGENTA(str) printf("\033[1;35m%s\033[0m", str)
#define CYAN(str) printf("\033[1;36m%s\033[0m", str)
#define WHITE(str) printf("\033[1;37m%s\033[0m", str)
#define RESET printf("\033[0m");


#define ERROR_MESSAGE(str)  printf("\033[1;31mError: %s (File: %s, Line: %d)\n\033[0m", str, __FILE__, __LINE__);
#define USAGE "Usage: ./ft_ping [-?] [-v] [-f] [-l] [-n] [-w] [-W] [-p] [-s] [-T] [-t] [--ip-timestamp] <hostname>\n"

#define HELP "Usage:\n ft_ping [OPTION]... <hostname>\n\n\n" \
"Send ICMP ECHO_REQUEST packets to network hosts.\n\n" \
"Options controlling ICMP request types:\n" \
"  --address              send ICMP_ADDRESS packets (root only)\n" \
"  --echo                 send ICMP_ECHO packets (default)\n" \
"  --mask                 same as --address\n" \
"  --timestamp            send ICMP_TIMESTAMP packets\n" \
"  -t                     send TYPE packets\n\n" \
" Options valid for all request types:\n" \
"  -n                     Do not resolve host addresses\n" \
"  -T <NUM>               Set type of service (TOS) to NUM\n" \
"  -v                     Verbose output\n" \
"  -w <NUM>               Stop after N seconds\n" \
"  -W <NUM>               Number of seconds to wait for response\n" \
" Options valid for --echo requests:\n" \
"  -f                     Flood ping (root only)\n" \
" --ip-timestamp=FLAG     IP timestamp of type FLAG, which is one of \"tsonly\" and \"tsaddr\"\n" \
"  -l <NUM>               Send NUMBER packets as fast as possible before falling into normal mode of behavior (root only)\n" \
"  -p <PATTERN>           Fill ICMP packet with given pattern\n" \
"  -s <NUM>               Send NUMBER data octets\n" \
"  -?                     Give this help list\n\n"
