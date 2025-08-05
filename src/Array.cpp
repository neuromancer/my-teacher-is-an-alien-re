#include "Array.h"

#include <stdio.h>
#include <string.h>
#include <windows.h>


/* Function start: 0x4249D0 */
void Array_Cleanup(void* array, unsigned int element_size, int num_elements, void (*cleanup_function)(void*)) {
    volatile int exception_state = 0;

    __try {
        exception_state = 1;

        // Calculate end position and modify array parameter directly
        // This should match: add dword ptr [ebp + 0x8],ecx from original
        array = (char*)array + (num_elements * element_size);

        // Main loop with direct parameter manipulation
        while (num_elements > 0) {
            // Modify array parameter directly: sub dword ptr [ebp + 0x8],eax
            array = (char*)array - element_size;

            cleanup_function(array);

            // Decrement parameter directly: dec dword ptr [ebp + 0x10]
            num_elements--;
        }

        exception_state = 0;
    }
    __except(EXCEPTION_EXECUTE_HANDLER) {
        // Exception handler
    }
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
