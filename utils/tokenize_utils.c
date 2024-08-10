#include "../includes/ping.h"
#include <ctype.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>

bool have_option(TokenArray *arr, TokenType type){
    for (int i = 0; i < arr->size; i++)
    {
        if (arr->tokens[i].type == type)
        {
            return true;
        }
    }
    return false;
}

TokenType get_option(TokenArray *arr, TokenType type){
    for (int i = 0; i < arr->size; i++)
    {
        if (arr->tokens[i].type == type)
        {
            return arr->tokens[i].type;
        }
    }
    return TokenType_None;
}

const char *get_option_value(TokenArray *arr, TokenType type){
    for (int i = 0; i < arr->size; i++)
    {
        if (pow(2, arr->tokens[i].type) == type)
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
            if (each_dot_part > 255){
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
    if (each_dot_part > 255 || nbr_of_dots != 3){
        return false;
    }
    my_memcpy(ping->ip_str, str, my_strlen(str) + 1);
    return true;
}

bool is_fdqn(char *str, ft_ping *ping)
{
    struct addrinfo hints, *res;
    int status;
    
    my_memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET;  // Sadece IPv4
    hints.ai_socktype = SOCK_STREAM;  // TCP bağlantı

        // getaddrinfo çağrısını yap
    if ((status = getaddrinfo(str, NULL, &hints, &res)) != 0) {
        return false;  // DNS çözümlemesi başarısız
    }

    struct sockaddr_in *ipv4 = (struct sockaddr_in *)res->ai_addr;
    void *addr = &(ipv4->sin_addr);

    // IP adresini string olarak dönüştür
    inet_ntop(res->ai_family, addr, ping->ip_str, sizeof ping->ip_str);

    freeaddrinfo(res);  // Bellek sızıntısını önlemek için bellek boşalt

    return true;
}