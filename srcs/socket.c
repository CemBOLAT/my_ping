#include "../includes/ping.h"
#include <netinet/in.h>
#include <netinet/ip.h>


void init_socket(ft_ping *ping){
    // create a raw socket for icmp protocol
    ping->socket = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
    if (ping->socket < 0){
        ERROR_MESSAGE("socket");
        ft_perfect_exit(ping);
    }
    // init dest_addr
    init_dest_addr(ping);
    
    // TIME TO LIVE

    // RECEIVE TIMEOUT
    struct timeval timeout;
    timeout.tv_sec = DEFAULT_TIMEOUT;
    timeout.tv_usec = 0;
    // SO_RCVTIMEO is used to set the timeout for the socket
    // SOL_SOCKET is the level of the socket option
    if (setsockopt(ping->socket, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout)) < 0){
        ERROR_MESSAGE("setsockopt");
        ft_perfect_exit(ping);
    }
    // SEND TIMEOUT

}