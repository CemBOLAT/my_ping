#pragma once

#include "parser.h"
#include <stdlib.h>

#define INITIAL_CAPACITY 10

typedef struct TokenArray {
    Token*  tokens;
    int     size;
    int     capacity;
} TokenArray;

TokenArray* createArray(){
    TokenArray* arr = (TokenArray*)malloc(sizeof(TokenArray));
    if (arr){
        ERROR_MESSAGE("Memory allocation failed");
        exit(1);
    }
    arr->size = 0;
    arr->capacity = INITIAL_CAPACITY;
    arr->tokens = (Token*)malloc(sizeof(Token) * arr->capacity);
    return arr;
}

void insertArray(TokenArray* arr, Token token){
    if (!arr){
        ERROR_MESSAGE("Array is NULL"); 
        exit(1); // sonrasında benim free fonksiyonumla değiştirilecek
    }
    if (arr->size >= arr->capacity){
        arr->capacity *= 2;
        arr->tokens = (Token*)realloc(arr->tokens, sizeof(Token) * arr->capacity);
    }
    arr->tokens[arr->size++] = token;
}

void freeArray(TokenArray* arr){
    if (!arr){
        ERROR_MESSAGE("Array is NULL");
        exit(1); // sonrasında benim free fonksiyonumla değiştirilecek
    }
    free(arr->tokens);
    free(arr);
}

void printArray(TokenArray* arr){
    if (!arr){
        ERROR_MESSAGE("Array is NULL");
        exit(1); // sonrasında benim free fonksiyonumla değiştirilecek
    }
    for (int i = 0; i < arr->size; i++){
        printf("Token Type: %d, Token Value: %s\n", arr->tokens[i].type, arr->tokens[i].value);
    }
}

void removeArrayIndex(TokenArray* arr, int index){
    if (!arr){
        ERROR_MESSAGE("Array is NULL");
        exit(1); // sonrasında benim free fonksiyonumla değiştirilecek
    }
    if (index < 0 || index >= arr->size){
        ERROR_MESSAGE("Index out of bounds");
        exit(1); // sonrasında benim free fonksiyonumla değiştirilecek
    }
    for (int i = index; i < arr->size - 1; i++){
        arr->tokens[i] = arr->tokens[i + 1];
    }
    arr->size--;
}

void removeArrayValue(TokenArray* arr, Token token){
    if (!arr){
        ERROR_MESSAGE("Array is NULL");
        exit(1); // sonrasında benim free fonksiyonumla değiştirilecek
    }
    for (int i = 0; i < arr->size; i++){
        if (arr->tokens[i].type == token.type && arr->tokens[i].value == token.value){
            removeArrayIndex(arr, i);
            return;
        }
    }
    ERROR_MESSAGE("Token not found");
    exit(1); // sonrasında benim free fonksiyonumla değiştirilecek
}