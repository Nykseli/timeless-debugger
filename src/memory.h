#ifndef REALTIME_DEBUG_MEMORY_H_
#define REALTIME_DEBUG_MEMORY_H_

#include <stdlib.h>

#define ALLOCATE(type, count) \
    (type*)reallocate(NULL, 0, sizeof(type) * (count))

#define FREE(type, pointer) \
    reallocate(pointer, sizeof(type), 0)

#define GROW_CAPACITY(capacity) \
    ((capacity) < 8 ? 8 : (capacity)*2)

#define GROW_ARRAY(previous, type, old_count, count)        \
    (type*)reallocate(previous, sizeof(type) * (old_count), \
        sizeof(type) * (count))

#define FREE_ARRAY(type, pointer, old_count) \
    reallocate(pointer, sizeof(type) * (old_count), 0)

#define ARRAY_APPEND(array, item, item_type, l_name)                                                        \
    do {                                                                                            \
        if ((array)->capacity < (array)->size + 1) {                                                \
            int _old_capacity = (array)->capacity;                                                  \
            (array)->capacity = GROW_CAPACITY(_old_capacity);                                       \
            (array)->l_name = GROW_ARRAY((array)->l_name, item_type, _old_capacity, (array)->capacity); \
        }                                                                                           \
        (array)->l_name[(array)->size] = item;                                                        \
        (array)->size++;                                                                            \
    } while (0)

void* reallocate(void* previous, size_t old_size, size_t new_size);

#endif
