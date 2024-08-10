#pragma once

#include "superArray.h"
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
    int numericValue;
} Options;

TokenArray* createArray();
void insertArray(TokenArray* arr, Token token);
void freeArray(TokenArray* arr);
void printArray(TokenArray* arr);
void removeArrayIndex(TokenArray* arr, int index);
void removeArrayValue(TokenArray* arr, Token token);

HostArray *createStringArray();
void insertStringArray(HostArray *arr, char *str);
void freeStringArray(HostArray *arr);
void printStringArray(HostArray *arr);
void removeStringArrayIndex(HostArray **arr, int index);
void removeStringArrayValue(HostArray *arr, char *str);

