#include <stdlib.h>

typedef struct {
    char* str;
    size_t memory_size;
} SString;

SString* sstring_malloc(size_t size);
void sstring_realloc(SString* str, size_t size);
SString* sstring_free(SString* str);
SString* sstring_cat(SString* str, char* str2);
SString* sstring_ncat(SString* str, char* str2, size_t n);
SString* sstring_nfree(SString* str, size_t n);
