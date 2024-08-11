# ft_ping Documentation

---

- [1. Overview](#1-overview)
- [2. Key Components](#2-key-components)
  - [2.1 IP Header (`iphdr`)](#21-ip-header-iphdr)
  - [2.2 ICMP Header (`icmphdr`)](#22-icmp-header-icmphdr)
  - [2.3 Packet Assembly](#23-packet-assembly)
- [3. Packet Structure](#3-packet-structure)
  - [3.1 IP Header](#31-ip-header)
  - [3.2 ICMP Header](#32-icmp-header)
  - [3.3 Payload (Optional)](#33-payload-optional)
- [4. Detailed Steps](#4-detailed-steps)
  - [4.1 Initialize Socket](#41-initialize-socket)
  - [4.2 Create IP Header](#42-create-ip-header)
  - [4.3 Create ICMP Header](#43-create-icmp-header)
  - [4.4 Data Section](#44-data-section)
  - [4.5 Checksum Calculation](#45-checksum-calculation)
  - [4.6 Add IP Timestamp Option](#46-add-ip-timestamp-option)
  - [4.7 Send Packet](#47-send-packet)
  - [4.8 Receive Packet](#48-receive-packet)
  - [4.9 Process Reply](#49-process-reply)
- [5. Packet Assembly Order](#5-packet-assembly-order)
  - [5.1 Example of Packet Sizes](#51-example-of-packet-sizes)
- [6. Flags and Options](#6-flags-and-options)
- [7. Example Workflow](#7-example-workflow)

---

## 1. Overview

The `ping` function in `inetutils-2.0` sends ICMP (Internet Control Message Protocol) echo request packets to a network host and receives echo reply packets to measure round-trip time and packet loss. This is commonly used for network diagnostics.

## 2. Key Components

### 2.1 IP Header (`iphdr`)

This is required for every packet sent over the network. It includes information such as the source and destination IP addresses, time-to-live (TTL), protocol (set to ICMP for ping), and more.

### 2.2 ICMP Header (`icmphdr`)

This header is specific to ICMP packets. For ping, it typically contains the type (ICMP_ECHO), code, checksum, identifier, and sequence number.

### 2.3 Packet Assembly

The `ping` function assembles packets by first creating the IP header, followed by the ICMP header, and then any additional data like timestamps or patterns, depending on the flags used.

## 3. Packet Structure

A typical ICMP Echo Request packet consists of three primary components:

### 3.1 IP Header

The IP header is typically 20 bytes long without any options. It contains the following fields:

- **Version (4 bits)**: Indicates the IP version (IPv4 = 4).
- **IHL (4 bits)**: Internet Header Length; the length of the IP header, typically 5 (5x4 = 20 bytes).
- **Type of Service (1 byte)**: Differentiated services, used for QoS.
- **Total Length (2 bytes)**: The total length of the IP packet (header + data).
- **Identification (2 bytes)**: Unique identifier for the packet.
- **Flags and Fragment Offset (3 bytes)**: Used for fragmentation.
- **Time to Live (TTL) (1 byte)**: Limits the packet's lifespan to avoid loops.
- **Protocol (1 byte)**: Indicates the next layer protocol (ICMP = 1).
- **Header Checksum (2 bytes)**: Error-checking for the header.
- **Source Address (4 bytes)**: IP address of the sender.
- **Destination Address (4 bytes)**: IP address of the receiver.
- **Options (Optional)**: Can vary in length; includes timestamp options if used.

### 3.2 ICMP Header

The ICMP header is typically 8 bytes long and contains the following fields:

- **Type (1 byte)**: Indicates the type of ICMP message (Echo Request = 8, Echo Reply = 0).
- **Code (1 byte)**: Provides additional context for the message type.
- **Checksum (2 bytes)**: Error-checking for the ICMP header and data.
- **Identifier (2 bytes)**: Used to match requests and replies (often the process ID).
- **Sequence Number (2 bytes)**: Incremented with each sent packet to track requests.

### 3.3 Payload (Optional)

The payload can include additional data, such as a specific pattern defined by the `-p` flag, or timestamps when using the `--ip-timestamp` option.

## 4. Detailed Steps

### 4.1 Initialize Socket

The `ping` function starts by initializing a raw socket that allows direct sending and receiving of network packets. This is done with:

```c
socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
```

### 4.2 Create IP Header

The IP header is constructed with fields like:

- **Version**: Typically set to 4 for IPv4.
- **Header Length**: Specifies the length of the IP header.
- **Type of Service (TOS)**: Can be set with the `-T` option to define the quality of service.
- **Total Length**: Size of the entire packet (IP header + ICMP header + data).
- **TTL**: Time-to-live for the packet; decrements at each hop and drops the packet when it reaches zero.
- **Protocol**: Set to `IPPROTO_ICMP` for ICMP packets.
- **Source and Destination IP**: The source is typically the sender's IP, and the destination is the target host.

### 4.3 Create ICMP Header

The ICMP header is then created with fields like:

- **Type**: Set to `ICMP_ECHO` (8) for echo request.
- **Code**: Set to 0 for echo requests.
- **Checksum**: Initially set to 0, then calculated over the entire ICMP message.
- **Identifier**: Typically set to the process ID (`getpid()`).
- **Sequence Number**: Incremented with each sent packet.

### 4.4 Data Section

If any additional data needs to be added (like a pattern specified with `-p`), it is added after the ICMP header. For example:

- **Pattern**: The `-p` flag can be used to fill the data section of the ICMP packet with a specific pattern.

### 4.5 Checksum Calculation

The checksum for the ICMP header and data is computed and filled into the ICMP header before sending the packet:

```c
icmp_hdr->checksum = checksum((uint16_t *)ping->packet, ping->packet_size);
```

### 4.6 Add IP Timestamp Option

If the `--ip-timestamp` flag is used, a timestamp option is added to the IP header:

- **tsonly**: Records only the time at each hop.
- **tsaddr**: Records both time and IP address at each hop.

This is handled by appending the timestamp option data to the IP header in the packet.

### 4.7 Send Packet

The assembled packet (IP header + ICMP header + data) is then sent through the raw socket:

```c
sendto(ping->socket, ping->packet, ping->packet_size, 0, (struct sockaddr *)&ping->dest_addr, sizeof(ping->dest_addr));
```

### 4.8 Receive Packet

The `recvfrom` function is used to receive incoming ICMP packets. The received packet is checked to ensure it is a valid echo reply and that the checksum matches.

### 4.9 Process Reply

The received ICMP packet is processed to extract the round-trip time (RTT), sequence number, TTL, and other information. This information is then printed out or used for statistics.

## 5. Packet Assembly Order

1. **IP Header**: Constructed first.
2. **ICMP Header**: Constructed second.
3. **Payload/Data Section**: Added last if applicable.

### 5.1 Example of Packet Sizes

- **IP Header**: Typically 20 bytes (without options).
- **ICMP Header**: Typically 8 bytes.
- **Payload/Data**: Varies based on input (e.g., 56 bytes for standard ping, customizable with `-s`).

## 6. Flags and Options

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

## 7. Example Workflow

1. **Initialize Socket**: Open a raw socket for sending ICMP packets.
2. **Create Headers**: Construct IP and ICMP headers, along with any additional options like `--ip-timestamp`.
3. **Send Packet**: Transmit the packet to the target host.
4. **Receive Response**: Await an ICMP Echo Reply and process it.
5. **Analyze Results**: Calculate and display round-trip times, packet loss, and other statistics.
