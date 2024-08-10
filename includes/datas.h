#pragma once

#include <netinet/ip_icmp.h> // icmp tanımı için gerekli
#include <netinet/ip.h> // iphdr tanımı için gerekli
#include <netinet/in.h> // sockaddr_in tanımı için gerekli
#include <arpa/inet.h> // inet_ntop
#include <stdbool.h> // bool tanımı için gerekli
#include <netinet/ip.h> // iphdr tanımı için gerekli
#include <netinet/in.h> // sockaddr_in tanımı için gerekli
#include <netdb.h> // addrinfo , getaddrinfo() , freeaddrinfo()

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
    HostArray           *hosts;
    Options             options[12];
    RoundTrip           round_trip;
    struct sockaddr_in  dest_addr;
    struct icmp         icmp_packet;
    char                *packet;
    int                 packet_size;
    int                 seq;

    bool                is_received;
    size_t              nbr_of_packets;
    char                ip_str[INET_ADDRSTRLEN];
    int                 parametersvalue;
    int                 socket;
} ft_ping;
