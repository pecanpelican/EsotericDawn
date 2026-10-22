#include "system/lathe_logger.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include "memory/lathe_memory.h"

int main(void){
	LATHE_INFO("Starting Arena Test...",nullptr);
    #ifdef LATHE_HEAP_BACKING_MEMORY_TEST
        void *backing_buf = malloc(LATHE_1KB);//platform_alloc(LATHE_1KB,true)
    #else
        unsigned char backing_buf[LATHE_1KB];
    #endif
    lathe_arena arena = {};
    arena_init(&arena, backing_buf, LATHE_1KB);
    int *z = (int *)arena_allocate(&arena, sizeof(int));
    if (z == nullptr) {
        printf("Theres nothing we can do\n");
    } else {
        *z = 1;
        printf("%p %d\n", (void *)z, *z);
    }
    
    [[maybe_unused]] void *should_fail = arena_allocate(&arena, SIZE_MAX);
    
    arena_free_all(&arena);
    
    printf("%p %c\n", (void *)z, *z);
    
    
    // lathe_vec3 v1 = {{2, 7, 1}};
    // lathe_vec3 v2 = {{8, 2, 8}};/*
    // -0.5f, -0.5f, 0.0f,
    //  0.5f, -0.5f, 0.0f,
    //  0.0f,  0.5f, 0.0f
    // */
    // assert(vec3_dot_product(&v1, &v2) == 38.0f);
    // fprintf(stderr, "dot is: %f\n", vec3_dot_product(&v1, &v2));
    //free(backing_buf); //not really needed 
    // fclose(fptr);
    LATHE_INFO("Ending Arena Test",nullptr);
    
}