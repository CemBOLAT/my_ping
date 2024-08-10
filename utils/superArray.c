#include "../includes/ping.h"
#include <string.h>
#include <signal.h>

TokenArray* createArray(){
    TokenArray* arr = (TokenArray*)malloc(sizeof(TokenArray));
    if (!arr){
        ERROR_MESSAGE("Memory allocation failed");
        signal_exit(SIGINT);
    }
    arr->size = 0;
    arr->capacity = INITIAL_CAPACITY;
    arr->tokens = (Token*)malloc(sizeof(Token) * arr->capacity);
    if (!arr->tokens){
        ERROR_MESSAGE("Memory allocation failed");
        signal_exit(SIGINT);
    }
    return arr;
}

void insertArray(TokenArray* arr, Token token){
    if (!arr){
        ERROR_MESSAGE("Array is NULL"); 
        signal_exit(SIGINT);
    }
    if (arr->size >= arr->capacity){
        arr->capacity *= 2;
        arr->tokens = (Token*)realloc(arr->tokens, sizeof(Token) * arr->capacity);
        if (!arr->tokens){
            ERROR_MESSAGE("Memory allocation failed");
            signal_exit(SIGINT);
        }
    }
    arr->tokens[arr->size++] = token;
}

void freeArray(TokenArray* arr){
    if (!arr){
        ERROR_MESSAGE("Array is NULL");
        signal_exit(SIGINT);
    }
    free(arr->tokens);
    free(arr);
}

void printArray(TokenArray* arr){
    if (!arr){
        ERROR_MESSAGE("Array is NULL");
        signal_exit(SIGINT);
    }
    for (int i = 0; i < arr->size; i++){
        printf("Token Type: %d, Token Value: %s\n", arr->tokens[i].type, arr->tokens[i].value);
    }
}

void removeArrayIndex(TokenArray* arr, int index){
    if (!arr){
        ERROR_MESSAGE("Array is NULL");
        signal_exit(SIGINT);
    }
    if (index < 0 || index >= arr->size){
        ERROR_MESSAGE("Index out of bounds");
        signal_exit(SIGINT);
    }
    for (int i = index; i < arr->size - 1; i++){
        arr->tokens[i] = arr->tokens[i + 1];
    }
    arr->size--;
}

void removeArrayValue(TokenArray* arr, Token token){
    if (!arr){
        ERROR_MESSAGE("Array is NULL");
        signal_exit(SIGINT);
    }
    for (int i = 0; i < arr->size; i++){
        if (arr->tokens[i].type == token.type && arr->tokens[i].value == token.value){
            removeArrayIndex(arr, i);
            return;
        }
    }
    ERROR_MESSAGE("Token not found");
    signal_exit(SIGINT);
}

HostArray *createStringArray(){
    HostArray *arr = (HostArray*)malloc(sizeof(HostArray));
    if (!arr){
        ERROR_MESSAGE("Memory allocation failed");
        signal_exit(SIGINT);;
    }
    arr->size = 0;
    arr->capacity = INITIAL_CAPACITY;
    arr->host = (char**)malloc(sizeof(char*) * arr->capacity);
    if (!arr->host){
        ERROR_MESSAGE("Memory allocation failed");
        signal_exit(SIGINT);;
    }
    return arr;
}

void insertStringArray(HostArray *arr, char *str){
    if (!arr){
        ERROR_MESSAGE("Array is NULL");
        signal_exit(SIGINT);
    }
    if (arr->size >= arr->capacity){
        arr->capacity *= 2;
        arr->host = (char**)realloc(arr->host, sizeof(char*) * arr->capacity);
        if (!arr->host){
            ERROR_MESSAGE("Memory allocation failed");
            signal_exit(SIGINT);
        }
    }
    arr->host[arr->size++] = str;
}

void freeStringArray(HostArray *arr){
    if (!arr){
        ERROR_MESSAGE("Array is NULL");
        signal_exit(SIGINT);
    }
    free(arr->host);
    free(arr);
}

void printStringArray(HostArray *arr){
    if (!arr){
        ERROR_MESSAGE("Array is NULL");
        signal_exit(SIGINT);
    }
    for (int i = 0; i < arr->size; i++){
        printf("Host: %s\n", arr->host[i]);
    }
}

void removeStringArrayIndex(HostArray **arr, int index){
    if (!arr){
        ERROR_MESSAGE("Array is NULL");
        signal_exit(SIGINT);
    }
    if (index < 0 || index >= (*arr)->size){
        ERROR_MESSAGE("Index out of bounds");
        signal_exit(SIGINT);
    }
    for (int i = index; i < (*arr)->size - 1; i++){
        (*arr)->host[i] = (*arr)->host[i + 1];
    }
    (*arr)->size--;
}

void removeStringArrayValue(HostArray *arr, char *str){
    if (!arr){
        ERROR_MESSAGE("Array is NULL");
        signal_exit(SIGINT);
    }
    for (int i = 0; i < arr->size; i++){
        if (my_strcmp(arr->host[i], str) == 0){
            removeStringArrayIndex(&arr, i);
            return;
        }
    }
    ERROR_MESSAGE("Host not found");
    signal_exit(SIGINT);
}

