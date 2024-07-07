#include "cache.h"
#include <string.h>

int hash(const char* key) {
    unsigned long hash = 5381;
    int c;
    while ((c = *key++))
        hash = ((hash << 5) + hash) + c;
    return hash % MAX_CACHE_SIZE;
}


LRUCache* init_cache() {
    LRUCache* cache = (LRUCache*) malloc(sizeof(LRUCache));
    cache->head = NULL;
    cache->tail = NULL;
    cache->size = 0;
    memset(cache->map, NULL, sizeof(cache->map));
    
    return cache;
}

void add_to_front(LRUCache* cache, CacheNode* node) {
    node->next = cache->head;
    node->prev = NULL;
    if (cache->head)
        cache->head->prev = node;
    cache->head = node;
    if (!cache->tail)
        cache->tail = node;
}

void remove_node(LRUCache* cache, CacheNode* node) {
    if (node->prev)
        node->prev->next = node->next;
    else
        cache->head = node->next;
    if (node->next)
        node->next->prev = node->prev;
    else
        cache->tail = node->prev;
}

char* get_from_cache(LRUCache* cache, const char* key) {
    int index = hash(key);
    CacheNode* node = cache->map[index];
    if (node && strcmp(node->key, key) == 0) {
        remove_node(cache, node);
        add_to_front(cache, node);
        return node->data;
    }
    return NULL;
}

void add_to_cache(LRUCache* cache, const char* key, char* data, size_t size) {
    CacheNode* node = (CacheNode*) malloc(sizeof(CacheNode));
    node->key = strdup(key);
    node->data = data;
    node->size = size;

    int index = hash(key);
    if (cache->map[index]) {
        remove_node(cache, cache->map[index]);
        free(cache->map[index]->key);
        free(cache->map[index]->data);
        free(cache->map[index]);
        cache->map[index] = NULL;
        cache->size--;
    }

    cache->map[index] = node;
    add_to_front(cache, node);
    cache->size++;

    if (cache->size > MAX_CACHE_SIZE) {
        CacheNode* lru = cache->tail;
        remove_node(cache, lru);
        int lruIndex = hash(lru->key);
        cache->map[lruIndex] = NULL;
        free(lru->key);
        free(lru->data);
        free(lru);
        cache->size--;
    }
}