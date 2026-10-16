#include "memory/lathe_memory.h"
#include "system/lathe_logger.h"
#include <assert.h>
#include <string.h>

// https://graphics.stanford.edu/~seander/bithacks.html#DetermineIfPowerOf2
[[maybe_unused]] static inline bool is_power_of_two(uintptr_t ptr_size) {
    return (ptr_size & (ptr_size - 1)) == 0;
}

void arena_init(lathe_arena *arena, void *backing_buf, size_t backing_capacity) {
    arena->buf = (unsigned char *)backing_buf;
    arena->buf_len = backing_capacity;
    arena->curr_offset = 0;
    arena->prev_offset = 0;
}

uintptr_t align_foward(uintptr_t ptr, size_t align) {
    uintptr_t p, a, modulo;
    assert(is_power_of_two(align));
    p = ptr;
    a = (uintptr_t)align;
    // https://graphics.stanford.edu/~seander/bithacks.html#ModulusDivisionEasy
    modulo = p & (a - 1); // p mod a quick, not too sure if the compiler would do this for me...hmm... to the GodBolt Mobile!
    if (modulo != 0) {
        p += a - modulo;
    }
    return p;
}

void *arena_alloced_align(lathe_arena *arena, size_t size, size_t align) {
    size_t result = {};
    uintptr_t curr = (uintptr_t)arena->buf + (uintptr_t)arena->curr_offset;
    uintptr_t offset = align_foward(curr, align);
    offset -= (uintptr_t)arena->buf; // cast the buf to ptr
    if (size > SIZE_MAX - offset) {
        LATHE_ERROR("The asked space would lead to an overflow\n", stderr);
        return nullptr;
    } else {
        result = offset + size;
    }
    if (result > arena->buf_len) {
        LATHE_WARN("The arena has no space for this, returning nullptr\n", stderr);
        return nullptr;
    }
    void *ptr = &arena->buf[offset];
    arena->prev_offset = offset;
    arena->curr_offset = result;
    
    memset(ptr, 0, size);
    return ptr;
}

//Note, need to cast since this isnt exactly malloc
void *arena_allocate(lathe_arena *arena, size_t size) {
    return arena_alloced_align(arena, size, LATHE_ALIGNMENT);
}

void arena_free_all(lathe_arena *arena) {
    arena->curr_offset = 0;
    arena->prev_offset = 0;
    #if defined(LATHE_ARENA_POISON)
        // https://www.aussieai.com/blog/poisoning-memory-safety
        memset(arena->buf, '@', arena->buf_len);
        LATHE_DEBUG("Arena buffer was posioned...bleh\n", stderr);
    #endif // defined(LATHE_ARENA_POISON)
}
