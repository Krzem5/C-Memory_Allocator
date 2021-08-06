#include <memory_allocator.h>
#include <stdint.h>
#include <stdlib.h>
#include <time.h>



#define ALLOCATION_COUNT 2048
#define MAX_ALLOCATION_SIZE 1024



typedef struct __BLOCK{
	uint8_t* p;
	uint32_t sz;
} block_t;



int main(int argc,const char** argv){
	init_allocator();
	srand((unsigned int)time(NULL));
	block_t bl[ALLOCATION_COUNT];
	for (uint32_t i=0;i<ALLOCATION_COUNT;i++){
		bl[i].sz=rand()*(MAX_ALLOCATION_SIZE-1)/RAND_MAX+1;
		bl[i].p=allocate(bl[i].sz);
		for (uint32_t j=0;j<bl[i].sz;j++){
			*(bl[i].p+j)=rand()*255/RAND_MAX;
		}
	}
	for (uint32_t i=0;i<(ALLOCATION_COUNT>>1);i++){
		uint32_t j=i+rand()/(RAND_MAX/(ALLOCATION_COUNT-i)+1);
		deallocate(bl[j].p);
		bl[j]=bl[i];
	}
	for (uint32_t i=0;i<(ALLOCATION_COUNT>>1);i++){
		bl[i].sz=rand()*(MAX_ALLOCATION_SIZE-1)/RAND_MAX+1;
		bl[i].p=allocate(bl[i].sz);
		for (uint32_t j=0;j<bl[i].sz;j++){
			*(bl[i].p+j)=rand()*255/RAND_MAX;
		}
	}
	for (uint32_t i=0;i<ALLOCATION_COUNT-1;i++){
		uint32_t j=i+rand()/(RAND_MAX/(ALLOCATION_COUNT-i)+1);
		deallocate(bl[j].p);
		bl[j]=bl[i];
	}
	deallocate(bl[ALLOCATION_COUNT-1].p);
	deinit_allocator();
	return 0;
}
