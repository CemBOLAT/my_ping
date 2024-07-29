#pragma once

#include "parser.h"
#include <stdlib.h>

#define INITIAL_CAPACITY 10

typedef struct TokenArray {
    Token*  tokens;
    int     size;
    int     capacity;
} TokenArray;


typedef struct HostArray {
    char    **host;
    int     size;
    int     capacity;
} HostArray;

typedef struct Options {
    char    *option;
    int     value;
} Options;

typedef struct s_ping {
    TokenArray *arr;
    int         mask;
    HostArray   *hosts;
    Options     options[12];
} ft_ping;

TokenArray* createArray();
void insertArray(TokenArray* arr, Token token);
void freeArray(TokenArray* arr);
void printArray(TokenArray* arr);
void removeArrayIndex(TokenArray* arr, int index);
void removeArrayValue(TokenArray* arr, Token token);
int parse_options(int argc, char **argv, ft_ping *ping);


HostArray *createStringArray();
void insertStringArray(HostArray *arr, char *str);
void freeStringArray(HostArray *arr);
void printStringArray(HostArray *arr);
void removeStringArrayIndex(HostArray **arr, int index);
void removeStringArrayValue(HostArray *arr, char *str);

void ft_perfect_exit(ft_ping *ping);