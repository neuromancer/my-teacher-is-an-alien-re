#ifndef ARRAY_H
#define ARRAY_H

void Array_Cleanup(void* array, unsigned int element_size, int num_elements, void (*cleanup_function)(void*));

#endif // ARRAY_H
