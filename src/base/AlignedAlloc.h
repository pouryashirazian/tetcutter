#include "MathBase.h"

#ifdef PS_OS_MAC
    #include <malloc/malloc.h>
#else
    #include <malloc.h>
#endif

#ifndef PS_L1_CACHE_LINE_SIZE
	#define PS_L1_CACHE_LINE_SIZE 64
#endif

#if defined(PS_OS_WINDOWS)
	#include "windows.h"
	#include "intrin.h"
#endif

inline void PS_PREFETCH(void *lpBuffer)
{
#if defined(PS_OS_WINDOWS)
	PreFetchCacheLine(PF_TEMPORAL_LEVEL_1, lpBuffer);
#else
	__builtin_prefetch(lpBuffer);
#endif
}

// Memory Allocation Functions
inline void *AllocAligned(U32 size)
{
#if defined(PS_OS_WINDOWS)
    return _aligned_malloc(size, PS_L1_CACHE_LINE_SIZE);
#elif defined (PS_OS_OPENBSD) || defined(PS_OS_MAC)
    // Allocate excess memory to ensure an aligned pointer can be returned
    void *mem = malloc(size + (PS_L1_CACHE_LINE_SIZE-1) + sizeof(void*));
    char *amem = ((char*)mem) + sizeof(void*);
#if (PS_POINTER_SIZE == 8)
    amem += PS_L1_CACHE_LINE_SIZE - (reinterpret_cast<uint64_t>(amem) &
    								(PS_L1_CACHE_LINE_SIZE - 1));
#else
    amem += PS_L1_CACHE_LINE_SIZE - (reinterpret_cast<uint32_t>(amem) &
                                       (PS_L1_CACHE_LINE_SIZE - 1));
#endif
    ((void**)amem)[-1] = mem;
    return amem;
#else
    return memalign(PS_L1_CACHE_LINE_SIZE, size);
#endif
}

template <typename T> T *AllocAligned(U32 count) {
    return (T *)AllocAligned(count * sizeof(T));
}


inline void FreeAligned(void *ptr)
{
    if (!ptr) return;
#if defined(PS_OS_WINDOWS)
    _aligned_free(ptr);
#elif defined (PS_OS_OPENBSD) || defined(PS_OS_APPLE)
    free(((void**)ptr)[-1]);
#else
    free(ptr);
#endif
}
