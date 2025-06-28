#include "string.h"

const char* strchr(const char *str, char chr)
{
    if(str == NULL) return NULL;

    while(*str){
        if(*str == chr) return str;

        str++;
    }

    return NULL;
}

char* strcpy(const char* src, char* dst)
{
    char* start = dst;

    if(dst == NULL){
        return NULL;
    }

    if(src == NULL){    
        
        *dst ='\0';
        return start;
    }


    while(*src){
        *dst = *src;
        src++;
        dst++;
    }

    *dst ='\0';
    return start;
}

unsigned strlen(const char* str){

    unsigned i = 0;
    while(*str){
        i++;
        str++;
    }
    return i;
}