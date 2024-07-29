#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <netinet/ip_icmp.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <errno.h>

// Hata mesajı ve çıkış fonksiyonu
void ERROR_MESSAGE(const char *message) {
    perror(message);
    exit(EXIT_FAILURE);
}

void ft_perfect_exit(void) {
    // Cleanup code here
    exit(EXIT_FAILURE);
}

// ICMP header için checksum hesaplama fonksiyonu
unsigned short checksum(void *b, int len) {    
    unsigned short *buf = b; 
    unsigned int sum = 0; 
    unsigned short result;

    for (sum = 0; len > 1; len -= 2)
        sum += *buf++;
    if (len == 1)
        sum += *(unsigned char *)buf;
    sum = (sum >> 16) + (sum & 0xFFFF);
    sum += (sum >> 16);
    result = ~sum;
    return result;
}

void init_socket(int *sock) {
    // Create a raw socket for ICMP protocol
    *sock = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
    if (*sock < 0) {
        ERROR_MESSAGE("socket");
        ft_perfect_exit();
    }

    // Set socket timeout
    struct timeval timeout;
    timeout.tv_sec = 1;  // 1 second
    timeout.tv_usec = 0;
    if (setsockopt(*sock, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout)) < 0) {
        ERROR_MESSAGE("setsockopt");
        ft_perfect_exit();
    }

    // Set socket TTL (optional)
    int ttl = 64;
    if (setsockopt(*sock, IPPROTO_IP, IP_TTL, &ttl, sizeof(ttl)) < 0) {
        ERROR_MESSAGE("setsockopt");
        ft_perfect_exit();
    }
}

void send_icmp_request(int sock, const char *hostname) {
    struct addrinfo hints, *res;
    struct sockaddr_in dest_addr;
    struct icmp icmp_hdr;
    char packet[sizeof(struct icmp)];

    // Resolve hostname to IP address
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_RAW;
    hints.ai_protocol = IPPROTO_ICMP;
    
    if (getaddrinfo(hostname, NULL, &hints, &res) != 0) {
        ERROR_MESSAGE("getaddrinfo");
        ft_perfect_exit();
    }
    
    dest_addr = *(struct sockaddr_in *)res->ai_addr;
    freeaddrinfo(res);

    // Prepare ICMP header
    memset(&icmp_hdr, 0, sizeof(icmp_hdr));
    icmp_hdr.icmp_type = ICMP_ECHO;
    icmp_hdr.icmp_code = 0;
    icmp_hdr.icmp_id = getpid();
    icmp_hdr.icmp_seq = 1;
    icmp_hdr.icmp_cksum = 0;
    icmp_hdr.icmp_cksum = checksum(&icmp_hdr, sizeof(icmp_hdr));

    // Copy ICMP header to packet
    memcpy(packet, &icmp_hdr, sizeof(icmp_hdr));

    // Send ICMP packet
    if (sendto(sock, packet, sizeof(packet), 0, (struct sockaddr *)&dest_addr, sizeof(dest_addr)) < 0) {
        ERROR_MESSAGE("sendto");
        ft_perfect_exit();
    }

    printf("ICMP request sent to %s\n", hostname);
}

void receive_icmp_reply(int sock) {
    char buffer[1024];
    struct sockaddr_in recv_addr;
    socklen_t addr_len = sizeof(recv_addr);
    int bytes_received = recvfrom(sock, buffer, sizeof(buffer), 0, (struct sockaddr *)&recv_addr, &addr_len);

    if (bytes_received < 0) {
        if (errno == EAGAIN || errno == EWOULDBLOCK) {
            printf("Request timed out.\n");
        } else {
            ERROR_MESSAGE("recvfrom");
            ft_perfect_exit();
        }
    } else {
        // Process ICMP reply
        struct iphdr *ip_hdr = (struct iphdr *)buffer;
        struct icmp *icmp_hdr = (struct icmp *)(buffer + (ip_hdr->ihl * 4));
        
        if (icmp_hdr->icmp_type == ICMP_ECHOREPLY) {
            printf("Received ICMP reply from %s\n", inet_ntoa(recv_addr.sin_addr));
        } else {
            printf("Received unexpected ICMP packet\n");
        }
    }
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <hostname>\n", argv[0]);
        return EXIT_FAILURE;
    }

    int sock;
    init_socket(&sock);
    send_icmp_request(sock, argv[1]);
    receive_icmp_reply(sock);

    close(sock);
    return EXIT_SUCCESS;
}
