#include "../includes/ping.h"
#include <ctype.h>
#include <stdbool.h>
#include <string.h>

TokenType get_option(TokenArray *arr, TokenType type){
    for (int i = 0; i < arr->size; i++)
    {
        if (arr->tokens[i].type == type) // Check if the token type is equal to the given type
        {
            return arr->tokens[i].type;
        }
    }
    return TokenType_None;
}

const char *get_option_value(TokenArray *arr, TokenType type){
    for (int i = 0; i < arr->size; i++)
    {
        if (my_pow(2, arr->tokens[i].type) == type) // Check if the token type is equal to the given type
        {
            return arr->tokens[i].value;
        }
    }
    return NULL;
}

bool isNumber(char *str){
    if (str[0] == '-')
    {
        str++;
    }
    for (int i = 0; str[i] != '\0'; i++)
    {
        if (!my_isDigit(str[i]))
        {
            return false;
        }
    }
    return true;
}

bool is_ipv4(char *str, ft_ping *ping){
    int each_dot_part = 0;
    int nbr_of_dots = 0;
    for (int i = 0; str[i] != '\0'; i++)
    {
        if (str[i] == '.'){
            if (each_dot_part > 255){ // Each part of the IP address should be less than 255
                return false;
            }
            each_dot_part = 0;
            nbr_of_dots++;
        }
        else if (!my_isDigit(str[i])){
            return false;
        }
        else{
            each_dot_part = each_dot_part * 10 + (str[i] - '0');
        }
    }
    if (each_dot_part > 255 || nbr_of_dots != 3){ // Each part of the IP address should be less than 255 and there should be 3 dots
        return false;
    }
    my_memcpy(ping->ip_str, str, my_strlen(str) + 1);
    return true;
}

// Bu fonksiyon, verilen string'in (str) bir Fully Qualified Domain Name (FQDN) olup olmadığını kontrol eder.
// Eğer FQDN ise, bu alan adını IPv4 adresine çevirir ve başarıyla çözülmüşse true, çözülmemişse false döner.
// FQDN: Tamamen Belirlenmiş Alan Adı, internette bir cihazın tam yerini belirten bir adrestir.
// Fonksiyon ayrıca çözülen IP adresini ping->ip_str içerisine kaydeder.
bool is_fdqn(char *str, ft_ping *ping) 
{
    struct addrinfo hints, *res; // getaddrinfo() için gerekli yapı: hints ayarları tutar, res sonuçları tutar
    int status;
    
    // hints yapısını sıfırlama, bu adım tüm alanları 0 ile doldurur
    my_memset(&hints, 0, sizeof hints);
    
    hints.ai_family = AF_INET;  // Yalnızca IPv4 adresleri ile ilgileniyoruz
    hints.ai_socktype = SOCK_STREAM;  // TCP bağlantıları için bu ayarı kullanıyoruz (UDP için SOCK_DGRAM kullanılabilir)

    // getaddrinfo: DNS çözümlemesi yaparak string'deki alan adını IP adresine çevirir
    // Eğer başarılı olursa, res pointer'ı sonuçları gösterir, başarısız olursa false döneriz
    if ((status = getaddrinfo(str, NULL, &hints, &res)) != 0) {
        // getaddrinfo başarısız olduysa, bu string geçerli bir FQDN değil demektir
        return false;
    }

    // IPv4 adresine sahip sonuç yapısına ulaş
    struct sockaddr_in *ipv4 = (struct sockaddr_in *)res->ai_addr; // IPv4 adres bilgisine ulaşmak için casting yapılır
    void *addr = &(ipv4->sin_addr); // IP adresini gösteren pointer (adres) elde edilir

    // inet_ntop: IP adresini string formatına çevirir ve ping->ip_str içerisine kaydeder
    inet_ntop(res->ai_family, addr, ping->ip_str, sizeof ping->ip_str);

    // Bellek sızıntısını önlemek için, getaddrinfo tarafından ayırılan belleği serbest bırakıyoruz
    freeaddrinfo(res);

    // FQDN çözümlemesi başarılı oldu ve IP adresi elde edildi, true döneriz
    return true;
}
