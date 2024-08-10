
#include "../includes/ping.h"

// Helper function to add IP timestamp option
void add_ip_timestamp_option(void *packet, int option_type) {
    struct iphdr *ip_hdr = (struct iphdr *)packet;
    uint8_t *options = (uint8_t *)(packet + sizeof(struct iphdr));
    int option_length = 0;

    // IP timestamp option header
    options[0] = 0x01; // Timestamp option type
    options[1] = 0x01; // Length (will be adjusted below)
    options[2] = option_type; // TSO: Timestamp only (1) or Timestamp + Address (2)
    options[3] = 0x00; // Reserved

    // Adjust option length and header fields
    if (option_type == 1) { // tsonly
        option_length = 8 + 4 * 9; // 8-byte header + 4 bytes per timestamp, up to 9 timestamps
    } else if (option_type == 2) { // tsaddr
        option_length = 8 + 8 * 4; // 8-byte header + 8 bytes per timestamp/address, up to 4 hops
    }

    options[1] = option_length; // Set the option length

    // Initialize the timestamp field
    my_memset(options + 4, 0, option_length - 4); // Fill with zeros

    ip_hdr->ihl = (sizeof(struct iphdr) + option_length) / 4;
    ip_hdr->tot_len = htons(ntohs(ip_hdr->tot_len) + option_length);
}