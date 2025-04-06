#ifndef CORE_UTILS_MEMORY_H
#define CORE_UTILS_MEMORY_H

#include <Arduino.h>
#include "./FirebaseConfig.h"
#if defined(ESP8266) && defined(MMU_EXTERNAL_HEAP)
#include <umm_malloc/umm_malloc.h>
#include <umm_malloc/umm_heap_select.h>
#if !defined(ESP8266_USE_EXTERNAL_HEAP)
#define ESP8266_USE_EXTERNAL_HEAP
#endif
#endif

class Memory
{
public:
    Memory() {}
    ~Memory() {}

    // Free reserved memory at pointer.
    void release(void *ptr)
    {
        void **p = reinterpret_cast<void **>(ptr);
        if (*p)
        {
            free(*p);
            *p = 0;
        }
    }

    void *alloc(size_t len, bool clear = true)
    {
        void *p = nullptr;
        size_t newLen = getReservedLen(len);
#if defined(BOARD_HAS_PSRAM)
        if (ESP.getPsramSize() > 0)
            p = reinterpret_cast<void *>(ps_malloc(newLen));
        else
            p = reinterpret_cast<void *>(malloc(newLen));

        if (!p)
            return NULL;
#else

#if defined(ESP8266_USE_EXTERNAL_HEAP)
        ESP.setExternalHeap();
#endif
        p = reinterpret_cast<void *>(malloc(newLen));
        bool nn = p ? true : false;
#if defined(ESP8266_USE_EXTERNAL_HEAP)
        ESP.resetHeap();
#endif

        if (!nn)
            return NULL;

#endif
        if (clear)
            memset(p, 0, newLen);
        return p;
    }

    size_t getReservedLen(size_t len)
    {
        int blen = len + 1;
        int newlen = (blen / 4) * 4;
        if (newlen < blen)
            newlen += 4;
        return (size_t)newlen;
    }
};
#endif