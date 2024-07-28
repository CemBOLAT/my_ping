#pragma once

#include "text_engine.h"

typedef enum TokenType {
    TokenType_Verbose, // -v // ayrıntı ekle
    TokenType_Help, // -? // yardım
    TokenType_Flood, // -f // paketi hızlı gönder fakat daha az ayrıntı
    TokenType_Preload, // -p // Önce belirli bir miktar veri gönder ve sonra gerçek veriyi gönder -l 10 ile 10 tane veri gönder
    TokenType_Numeric, // -n // IP adreslerini sayısal olarak göster DNS çözümleme yapma
    TokenType_Deadline, // -w // Zaman aşımı süresi
    TokenType_Timeout, // -W // Zaman aşımı süresi
    TokenType_Pattern, // -p // Gönderilen paketlerin veri bölümüne bir model (pattern) ekler. Örneğin, -p ff her paket verisini ff ile doldurur.
    TokenType_PacketSize, // -s // Paket boyutu -s 100 100 baytlık ICMP paketleri gönderir.
    TokenType_TimeStamp, // -T // Zaman damgası ekle
    TokenType_TimeToLive, // -t // TTL değeri bu paketin kaç hop geçebileceğini belirler
    TokenType_Ip_TimeStamp, // --ip-timestamp // IP zaman damgası ekle  Bu, paketlerin ne zaman gönderildiği ve alındığı hakkında bilgi sağlar.
    /*
        Mixtape Usage of the ping commands parameters
        -v : Tüm parametreler ile beraber kullanılabilir.
        Parametre	    Açıklama	                            Değişken	                    Değişken Sayısı
        -v	            Ayrıntılı çıktı üretir.	                Almaz	                        0
        -?	            Yardım bilgilerini gösterir.	        Almaz	                        0
        -f	            Hedefe hızlı paket gönderir.	        Almaz	                        0
        -l	            Önce belirli miktar veri gönderir.	    Gönderilecek paket sayısı	    1
        -n	            Sayısal IP adresleri gösterir.	        Almaz	                        0
        -w	            Zaman aşımı süresi belirler.	        Zaman aşımı süresi (saniye)	    1
        -W	            Her paket için bekleme süresi.	        Bekleme süresi (saniye)	        1
        -p              (Pattern)	Veri bölümüne model ekler.	Model (pattern)	                1
        -s	            Paket boyutunu belirler.	            Paket boyutu (bayt)	            1
        -T	            Zaman damgası ekler.	                Almaz	                        0
        -t	            TTL değerini belirler.	                TTL değeri	                    1
        --ip-timestamp	IP zaman damgası ekler.	                Almaz	                        0
    */
}TokenType;

typedef struct Token {
    TokenType type;
    char* value; // -l 10 -> value = 10 for -v value = NULL
}Token;