#include "FreeRTOS.h"

// This will make C++ classes use the FreeRTOS heap
// instead of standard/pico-sdk provided allocators

void *operator new(size_t size) { return pvPortMalloc(size); }

void *operator new[](size_t size) { return pvPortMalloc(size); }

void operator delete(void *ptr) noexcept { vPortFree(ptr); }

void operator delete[](void *ptr) noexcept { vPortFree(ptr); }

void operator delete(void *ptr, size_t size) noexcept {
    (void)size;
    vPortFree(ptr);
}

void operator delete[](void *ptr, size_t size) noexcept {
    (void)size;
    vPortFree(ptr);
}