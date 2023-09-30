#include "FreeRTOS.h"
#include <cstring> // memcpy

void* operator new(size_t size) { 
    return pvPortMalloc(size); 
}

void operator delete(void *ptr) { 
    vPortFree(ptr); 
}

void operator delete(void *ptr, size_t) { 
    vPortFree(ptr); 
}

void* malloc(size_t size) { 
    return pvPortMalloc(size); 
}

void* calloc(size_t nItems, size_t itemSize) { 
    return pvPortMalloc(nItems * itemSize); 
}

void free(void* ptr) { 
    vPortFree(ptr); 
}

void* realloc(void* ptr, size_t newSize) {
    auto res = malloc(newSize);
    if (res && ptr) {
        memcpy(res, ptr, newSize);
        free(ptr);
    }
    return res;
}
