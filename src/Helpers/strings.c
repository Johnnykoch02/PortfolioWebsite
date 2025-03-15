#include "strings.h"
#include <string.h>

SString* sstring_malloc(size_t size) {
    SString* str = (SString*) malloc(sizeof(SString));
    str->str = (char*) malloc(size);
    str->str[0] = '\0';
    str->memory_size = size;
    return str;
}

void sstring_realloc(SString* str, size_t size) {
    char* new_str = (char*) malloc(size);
    new_str[0] = '\0';
    strcpy(new_str, str->str);
    free(str->str);
    str->str = new_str;
    str->memory_size = size;
    return str;
}

SString* sstring_free(SString* str) {
    free(str->str);
    free(str);
    return NULL;
}

SString* sstring_cat(SString* str, char* str2) {
    size_t len2 = strlen(str2);
    size_t len1 = strlen(str->str);
    
    if (len1 + len2 + 1 > str->memory_size) {
        sstring_realloc(str, (len1 + len2) * 2);
    }
    
    strcat(str->str, str2);
    return str;
}

SString* sstring_ncat(SString* str, char* str2, size_t n) {
    size_t len1 = strlen(str->str);
    
    if (len1 + n + 1 > str->memory_size) {
        sstring_realloc(str, (len1 + n) * 2);
    }
    
    strncat(str->str, str2, n);
    return str;
}

SString* sstring_nfree(SString* str, size_t n) {
    size_t len = strlen(str->str);
    
    if (n >= len) {
        return sstring_free(str);
    }
    
    str->str[len - n] = '\0';
    return str;
}