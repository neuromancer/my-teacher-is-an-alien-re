#include "Array.h"

#include <stdio.h>
#include <string.h>
#include <windows.h>


/* Function start: 0x4249D0 */
void Array_Cleanup(void* array, unsigned int element_size, int num_elements, void (*cleanup_function)(void*)) {
    int exception_state = 0;

    __try {
        // Calculate end position
        (char*&)array += num_elements * element_size;

        // Pre-decrement loop
        num_elements--;
        if (num_elements >= 0) {
            do {
                (char*&)array -= element_size;
                cleanup_function(array);
                num_elements--;
            } while (num_elements >= 0);
        }

        exception_state = 1;
    }
    __except(EXCEPTION_EXECUTE_HANDLER) {
    }

    if (exception_state != 0) {
        return;
    }
    Array_Cleanup(array, element_size, num_elements, cleanup_function);
}

/* Function start: 0x424b00 */
void Array_Iterate(void* array, unsigned int element_size, int num_elements, void (*callback)(void*), void (*cleanup_function)(void*))
{
    volatile int i = 0;
    //volatile int normal_completion = 0;

    __try {
        if (0 < num_elements) {
            do {
                callback(array);
                //cleanup_function(array);

                array = (char*)array + element_size;
                i++;
            } while (num_elements > i);
        }
        //normal_completion = 1;
    }
    __except(EXCEPTION_EXECUTE_HANDLER) {
        /*if (normal_completion == 0) {
            // The compiler will automatically generate the call to the unwind function.
        }*/
    }
}
