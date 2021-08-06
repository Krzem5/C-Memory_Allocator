#ifndef __MEMORY_ALLOCATOR_H__
#define __MEMORY_ALLOCATOR_H__ 1
#include <stddef.h>
#include <stdint.h>



void init_allocator(void);



void* allocate(uint32_t sz);



void deallocate(void* p);



void deinit_allocator(void);



#endif
