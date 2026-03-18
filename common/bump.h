#pragma once

/* pick -DBUMP_USE_UNIX or one of: -DUSE_EMBEDDED / -DBUMP_USE_EMBEDDED */
#if !defined(BUMP_USE_UNIX) && !defined(USE_EMBEDDED) && !defined(BUMP_USE_EMBEDDED)
  #define BUMP_USE_UNIX   /* default to unix */
#endif

#include <stdint.h>
#include <stddef.h>

#ifdef BUMP_USE_UNIX
  #include <unistd.h>
#endif

#if defined(USE_EMBEDDED) || defined(BUMP_USE_EMBEDDED)
  #ifndef ALLOCATOR_HEAP_SIZE
    #define ALLOCATOR_HEAP_SIZE 8192
  #endif
#endif

void* bump_allocator(intptr_t size);
