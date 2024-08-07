#include "../includes/ping.h"
#include <ctype.h>
#include <stdbool.h>

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
        if (arr->tokens[i].type == type)
        {
            return arr->tokens[i].value;
        }
    }
    return NULL;
}

bool isNumber(char *str){
    for (int i = 0; str[i] != '\0'; i++)
    {
        if (!isdigit(str[i]))
        {
            return false;
        }
    }
    return true;
}

bool is_ipv4(char *str){
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
        else if (!isdigit(str[i])){
            return false;
        }
        else{
            each_dot_part = each_dot_part * 10 + (str[i] - '0');
        }
    }
    if (each_dot_part > 255 || nbr_of_dots != 3){
        return false;
    }
    return true;
}