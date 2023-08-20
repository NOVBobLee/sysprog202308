#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>

// alignment != 0
static inline uintptr_t align_up(uintptr_t sz, size_t alignment)
{
    uintptr_t mask = alignment - 1;
    if ((alignment & mask) == 0) { /* power of two? */
        return (sz + mask) & ~mask;
    }
    return (((sz + mask) / alignment) * alignment);
}

int main(void)
{
    size_t alignment = 4;
    uintptr_t last = 0;
    for (uintptr_t sz = 0; sz < 128; ++sz) {
        uintptr_t output = align_up(sz, alignment);
        if (last != output)
            printf("-------------------\n");
        printf("sz: %" PRIuPTR " align: %zu output: %" PRIuPTR "\n", sz,
               alignment, output);
        last = output;
    }
    return 0;
}
