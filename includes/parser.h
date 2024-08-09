#pragma once

#include "text_engine.h"

typedef enum s_TokenType {
    TokenType_None        = 0,    // No flag
    TokenType_Verbose     = 1,    // 1 << 0
    TokenType_Help        = 2,    // 1 << 1
    TokenType_Flood       = 4,    // 1 << 2
    TokenType_Preload     = 8,    // 1 << 3
    TokenType_Numeric     = 16,   // 1 << 4
    TokenType_Deadline    = 32,   // 1 << 5
    TokenType_Timeout     = 64,   // 1 << 6
    TokenType_Pattern     = 128,  // 1 << 7
    TokenType_PacketSize  = 256,  // 1 << 8
    TokenType_TimeStamp   = 512,  // 1 << 9
    TokenType_TimeToLive  = 1024, // 1 << 10
    TokenType_Ip_TimeStamp= 2048  // 1 << 11
    /*
        Mixtape Usage of the ping commands parameters
        -v : Tüm parametreler ile beraber kullanılabilir.
        Parametre	    Açıklama	                            Değişken	                    Değişken Sayısı
        -v	            Ayrıntılı çıktı üretir.	                Almaz	                        0 (done)
        -?	            Yardım bilgilerini gösterir.	        Almaz	                        0 (done)
        -f	            Hedefe hızlı paket gönderir.	        Almaz	                        0 (done)
        -l	            Önce belirli miktar veri gönderir.	    Gönderilecek paket sayısı	    1 (done)
        -n	            Sayısal IP adresleri gösterir.	        Almaz	                        0
        -w	            stop after N seconds	                Zaman aşımı süresi (saniye)	    1 (half done)
        -W	            number of seconds to wait for response	        Bekleme süresi (saniye)	        1
        -p              (Pattern)	Veri bölümüne model ekler.	Model (pattern)	                1
        -s	            Paket boyutunu belirler.	            Paket boyutu (bayt)	            1 (done)
        -T	            Zaman damgası ekler.	                Almaz	                        0 
        -t	            TTL değerini belirler.	                TTL değeri	                    1
        --ip-timestamp	IP zaman damgası ekler.	                Almaz	                        0
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
    void *function; // -v -> function = verbose
}Token;