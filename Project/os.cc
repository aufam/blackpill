#include "os.h"

/// overload operator new with malloc from heap_4
void *operator new(size_t size) { return pvPortMalloc(size); }

/// overload operator delete with free from heap_4
void operator delete(void *ptr) { vPortFree(ptr); }