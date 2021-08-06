#ifdef _MSC_VER
#define WIN32_LEAN_AND_MEAN 1
#include <windows.h>
#else
#error Unimplemented!
#endif
#include <memory_allocator.h>
#include <signal.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>



#define _ASSERT_STR_(x) #x
#define _ASSERT_STR(x) _ASSERT_STR_(x)
#define ASSERT(x) \
	do{ \
		if (!(x)){ \
			printf("File \"%s\", Line %u (%s): %s: Assertion Failed\n",__FILE__,__LINE__,__func__,_ASSERT_STR(x)); \
			raise(SIGABRT); \
		} \
	} while (0)

#define CORRECT_ALIGNMENT(n) ASSERT(!(((uint64_t)(n))&(ALLOCATOR_ALIGNMENT-1)))

#define ALIGN(a) ((((uint64_t)(a))+ALLOCATOR_ALIGNMENT-1)&(~(ALLOCATOR_ALIGNMENT-1)))

#define ALLOCATOR_SIZE 1048576
#define ALLOCATOR_ALIGNMENT 8

#define SIZE_MASK 0xfffffffffffffffe
#define FLAG_USED 1

#define PAGE_RESERVATION_COUNT 16



typedef struct __HEADER{
	uint64_t sz;
} header_t;



typedef struct __NODE{
	uint64_t sz;
	struct __NODE* p;
	struct __NODE* n;
} node_t;



typedef struct __ALLOCATOR{
	node_t* h;
} allocator_t;



uint64_t _pg_sz;
allocator_t a_dt;



void init_allocator(void){
#ifdef _MSC_VER
	SYSTEM_INFO si;
	GetSystemInfo(&si);
	_pg_sz=si.dwPageSize;
#else
#endif
	a_dt.h=NULL;
}



void* allocate(uint32_t sz){
	sz=ALIGN(sz)+sizeof(header_t);
	if (sz<sizeof(node_t)){
		sz=sizeof(node_t);
	}
	CORRECT_ALIGNMENT(sz);
	node_t* n=a_dt.h;
	while (n){
		if (n->sz>sz){
			break;
		}
		n=n->n;
	};
	if (!n){
		uint64_t pg_sz=(sz+_pg_sz-1)/_pg_sz*_pg_sz;
#ifdef _MSC_VER
		void* pg=VirtualAlloc(NULL,pg_sz,MEM_COMMIT|MEM_RESERVE,PAGE_READWRITE);
#else
#endif
		CORRECT_ALIGNMENT(pg);
		n=(node_t*)pg;
		n->sz=pg_sz-sizeof(header_t);
		n->p=NULL;
		n->n=a_dt.h;
		if (n->n){
			n->n->p=n;
		}
		((header_t*)((uint64_t)pg+pg_sz-sizeof(header_t)))->sz=FLAG_USED;
		a_dt.h=n;
	}
	if (n->sz-sz>=sizeof(node_t)){
		node_t* nn=(node_t*)((uint64_t)n+sz);
		CORRECT_ALIGNMENT(nn);
		nn->sz=n->sz-sz;
		nn->p=n->p;
		nn->n=n->n;
		if (!nn->p){
			a_dt.h=nn;
		}
		else{
			nn->p->n=nn;
		}
		if (nn->n){
			nn->n->p=nn;
		}
	}
	else{
		sz=(uint32_t)n->sz;
		CORRECT_ALIGNMENT(sz);
		if (!n->p){
			a_dt.h=n->n;
		}
		else{
			n->p->n=n->n;
		}
		if (n->n){
			n->n->p=n->p;
		}
	}
	header_t* h=(header_t*)n;
	h->sz=sz|FLAG_USED;
	return (void*)((uint64_t)h+sizeof(header_t));
}



void deallocate(void* p){
	node_t* n=(node_t*)((uint64_t)p-sizeof(header_t));
	n->sz&=SIZE_MASK;
	node_t* nn=(node_t*)((uint64_t)n+n->sz);
	if (nn->sz&FLAG_USED){
		n->p=NULL;
		n->n=a_dt.h;
		a_dt.h=n;
	}
	else{
		n->sz+=nn->sz;
		n->p=nn->p;
		n->n=nn->n;
		if (n->p){
			n->p->n=n;
		}
	}
	if (n->n){
		n->n->p=n;
	}
}



void deinit_allocator(void){
}
