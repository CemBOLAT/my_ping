# ft_ping

## Detailed Explanation of `ping` Function in `inetutils-2.0`

The `ping` function in `inetutils-2.0` sends ICMP (Internet Control Message Protocol) echo request packets to a network host and receives echo reply packets to measure round-trip time and packet loss. This is commonly used for network diagnostics.

## Key Components

1. **IP Header (`iphdr`)**: This is required for every packet sent over the network. It includes information such as the source and destination IP addresses, time-to-live (TTL), protocol (set to ICMP for ping), and more.

2. **ICMP Header (`icmphdr`)**: This header is specific to ICMP packets. For ping, it typically contains the type (ICMP_ECHO), code, checksum, identifier, and sequence number.

3. **Packet Assembly**: The `ping` function assembles packets by first creating the IP header, followed by the ICMP header, and then any additional data like timestamps or patterns, depending on the flags used.

## Detailed Steps

### 1. Initialize Socket
The `ping` function starts by initializing a raw socket that allows direct sending and receiving of network packets. This is done with:
```c
socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
```

### 2. Create IP Header
The IP header is constructed with fields like:
- **Version**: Typically set to 4 for IPv4.
- **Header Length**: Specifies the length of the IP header.
- **Type of Service (TOS)**: Can be set with the `-T` option to define the quality of service.
- **Total Length**: Size of the entire packet (IP header + ICMP header + data).
- **TTL**: Time-to-live for the packet; decrements at each hop and drops the packet when it reaches zero.
- **Protocol**: Set to `IPPROTO_ICMP` for ICMP packets.
- **Source and Destination IP**: The source is typically the sender's IP, and the destination is the target host.

### 3. Create ICMP Header
The ICMP header is then created with fields like:
- **Type**: Set to `ICMP_ECHO` (8) for echo request.
- **Code**: Set to 0 for echo requests.
- **Checksum**: Initially set to 0, then calculated over the entire ICMP message.
- **Identifier**: Typically set to the process ID (`getpid()`).
- **Sequence Number**: Incremented with each sent packet.

### 4. Data Section
If any additional data needs to be added (like a pattern specified with `-p`), it is added after the ICMP header. For example:
- **Pattern**: The `-p` flag can be used to fill the data section of the ICMP packet with a specific pattern.

### 5. Checksum Calculation
The checksum for the ICMP header and data is computed and filled into the ICMP header before sending the packet:
```c
icmp_hdr->checksum = checksum((uint16_t *)ping->packet, ping->packet_size);
```

### 6. Add IP Timestamp Option
If the `--ip-timestamp` flag is used, a timestamp option is added to the IP header:
- **tsonly**: Records only the time at each hop.
- **tsaddr**: Records both time and IP address at each hop.

This is handled by appending the timestamp option data to the IP header in the packet.

### 7. Send Packet
The assembled packet (IP header + ICMP header + data) is then sent through the raw socket:
```c
sendto(ping->socket, ping->packet, ping->packet_size, 0, (struct sockaddr *)&ping->dest_addr, sizeof(ping->dest_addr));
```

### 8. Receive Packet
The `recvfrom` function is used to receive incoming ICMP packets. The received packet is checked to ensure it is a valid echo reply and that the checksum matches.

### 9. Process Reply
The received ICMP packet is processed to extract the round-trip time (RTT), sequence number, TTL, and other information. This information is then printed out or used for statistics.

## Flags and Options

- **`-v`**: Enables verbose output, providing additional details about each packet sent and received.
- **`-?`**: Displays help information.
- **`-f`**: Floods the target with packets, sending them as fast as possible.
- **`-l`**: Preloads a specified number of packets before beginning normal pinging.
- **`-n`**: Disables name resolution, displaying IP addresses instead of hostnames.
- **`-w`**: Sets a deadline, in seconds, for how long to run the ping.
- **`-W`**: Sets a timeout for each packet, in seconds.
- **`-p`**: Fills the ICMP data section with a specified pattern.
- **`-s`**: Specifies the size of the ICMP packet to send.
- **`-T`**: Sets the TOS (Type of Service) field in the IP header.
- **`-t`**: Specifies the type of ICMP packet to send, like echo request, timestamp request, etc.
- **`--ip-timestamp`**: Adds an IP timestamp option to record the time and optionally the IP address of each hop.

## Example Workflow

1. **User runs the `ping` command with specific options**:
   ```
   ping -v -s 56 --ip-timestamp=tsonly www.example.com
   ```

2. **The function initializes the socket and constructs the IP and ICMP headers**.
3. **The headers and additional data are combined into a packet**.
4. **The checksum is calculated, and the packet is sent**.
5. **The function waits for a reply, validates it, and processes the results**.
6. **Output is displayed to the user, showing RTT, TTL, and other relevant data**.
