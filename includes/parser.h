#pragma once

#include "text_engine.h"

typedef enum s_TokenType {
    TokenType_Verbose     = 1,    // 1 << 0 -v (done)
    TokenType_Help        = 2,    // 1 << 1 -? (done)
    TokenType_Flood       = 4,    // 1 << 2 -f (done)
    TokenType_Preload     = 8,    // 1 << 3 -l (done)
    TokenType_Numeric     = 16,   // 1 << 4 -n 
    TokenType_Deadline    = 32,   // 1 << 5 -w
    TokenType_Timeout     = 64,   // 1 << 6 -W
    TokenType_Pattern     = 128,  // 1 << 7 -p 
    TokenType_PacketSize  = 256,  // 1 << 8 -s (done)
    TokenType_TOS         = 512,  // 1 << 9 -T
    TokenType_SendPacketType  = 1024, // 1 << 10 -t
    TokenType_Ip_TimeStamp= 2048,  // 1 << 11 --ip-timestamp
    TokenType_None        = 0      // No flag
    /*
        Mixtape Usage of the ping commands parameters
        -v : Tüm parametreler ile beraber kullanılabilir.
        Parametre	    Açıklama	                            Değişken	                    Değişken Sayısı
        -v	            Ayrıntılı çıktı üretir.	                Almaz	                        0 (done)
        -?	            Yardım bilgilerini gösterir.	        Almaz	                        0 (done)
        -f	            Hedefe hızlı paket gönderir.	        Almaz	                        0 (done)
        -l	            Önce belirli miktar veri gönderir.	    Gönderilecek paket sayısı	    1 (done)
        -n	            do not resolve host addresses	        Almaz	                        0 (no need to done)
        -w	            stop after N seconds	                Zaman aşımı süresi (saniye)	    1 (done)
        -W	            number of seconds to wait for response	        Bekleme süresi (saniye)	1 (done)
        -p              (Pattern)	Veri bölümüne model ekler.	Model (pattern)	                1 (done)
        -s	            Paket boyutunu belirler.	            Paket boyutu (bayt)	            1 (done)
        -T	            set type of service (TOS) to NUM        alır                            1  (done)
        -t	            send paket types                        alır                            1 (done)
        --ip-timestamp	IP zaman damgası ekler.	                alır(tsonly and tsaddr)         1
    */
    /*
        -t için
        echo: Sends ICMP echo request packets (the default).
        address: Sends ICMP address mask requests.
        mask: Same as address, sends ICMP address mask requests.
        timestamp: Sends ICMP timestamp requests.


        When you use --ip-timestamp=tsonly, the IP packets sent by ping will include a timestamp option that only records the time at which the packet reaches each hop along its route. This option can record up to nine timestamps, providing information on how long the packet took to reach each hop.
tsaddr:

When you use --ip-timestamp=tsaddr, the IP packets will include a timestamp option that records both the time and the IP address of each hop the packet passes through. However, this option can only record up to four hops due to the additional space required for storing IP addresses.
    */
}TokenType;


// Setsockopt parameters.
#define DEFAULT_DEADLINE -1


#define DEFAULT_TIMEOUT 1
#define DEFAULT_TIME_TO_LIVE 64

#define DEFAULT_PACKET_SIZE 56

typedef struct Token {
    TokenType type;
    char* value; // -l 10 -> value = 10 for -v value = NULL
}Token;


#include "datas.h"

TokenType   get_option(TokenArray *arr, TokenType type);
const char  *get_option_value(TokenArray *arr, TokenType type);
int         parse_options(int argc, char **argv, ft_ping *ping);
