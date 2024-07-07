#ifndef __CACHE_H__
#define __CACHE_H__

#include <stdlib.h>

#define MAX_CACHE_SIZE 32

typedef struct CacheNode {
    char* key;
    char* data;
    size_t size;
    struct CacheNode* prev;
    struct CacheNode* next;
} CacheNode;

typedef struct {
    CacheNode* head;
    CacheNode* tail;
    int size;
    CacheNode* map[MAX_CACHE_SIZE];
} LRUCache;

LRUCache* init_cache();

char* get_from_cache(LRUCache* cache, const char* key);

void add_to_cache(LRUCache* cache, const char* key, char* data, size_t size);

#endif