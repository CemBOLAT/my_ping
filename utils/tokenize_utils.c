#include "../includes/ping.h"
#include <ctype.h>

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