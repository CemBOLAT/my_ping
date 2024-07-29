#include "../includes/ping.h"

static void setsockopt_custom(ft_ping *ping, int level, int option, TokenType type, int default_value){
    printf("setsockopt_custom\n");
    int tmp = get_option(ping->arr, type);
    int value = (tmp == TokenType_None) ? default_value : tmp;
    if (setsockopt(ping->socket, level, option, &value, sizeof(value)) < 0){
        ERROR_MESSAGE("setsockopt");
        ft_perfect_exit(ping);
    }
}

void init_socket(ft_ping *ping){
    // create a raw socket for icmp protocol
    ping->socket = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
    if (ping->socket < 0){
        ERROR_MESSAGE("socket");
        ft_perfect_exit(ping);
    }
    // set the socket options

    // TIME TO LIVE
    setsockopt_custom(ping, IPPROTO_IP, IP_TTL, TokenType_TimeToLive, DEFAULT_TIME_TO_LIVE);

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
    //setsockopt_custom(ping, SOL_SOCKET, SO_SNDTIMEO, TokenType_Deadline, DEFAULT_DEADLINE);

    // --ip-timestamp option is used to add timestamp to the ip header
    if (get_option(ping->arr, TokenType_Ip_TimeStamp) != TokenType_None) {
        int option = 1;
        if (setsockopt(ping->socket, IPPROTO_IP, ICMP_TIMESTAMP, &option, sizeof(option)) < 0) {
            perror("setsockopt");
            exit(EXIT_FAILURE);
        }
    }
}