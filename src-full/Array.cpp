#include "Array.h"

#include <stdio.h>
#include <string.h>
#include <windows.h>

// THESE TWO ARE FUNCTIONS PRODUCED BY THE COMPILER FOR DELETING ARRAYS OF OBJECTS
// THESE SHOULD BE REMOVED FROM HERE, DO NOT CHANGE

/* Function start: 0x4249D0 */ /* DEMO ONLY - no full game match */
void Array_Cleanup(void* array, unsigned int element_size, int num_elements, void (*cleanup_function)(void*)) {
    // Match MSVC 4.2 stack/register locals
    char* _array = (char*)array;                 // maps to EBX
    int _num_elements = num_elements;            // maps to EDI
    unsigned int _element_size = element_size;   // maps to ESI
    void (*_cleanup_function)(void*) = cleanup_function; // maps to EBP
    int _exception_state = 0;                    // inline SEH signal

    // Compute end pointer (array + num_elements * element_size)
    _array += _num_elements * _element_size;

    // Pre-decrement loop
    _num_elements--;
    if (_num_elements >= 0) {
        do {
            _array -= _element_size;          // move to previous element
            _cleanup_function(_array);        // call cleanup function
            _num_elements--;
        } while (_num_elements >= 0);
    }

    // Signal success (used by inline SEH in calling destructor)
    _exception_state = 1;

    // No recursion, no __try/__except; MSVC 4.2 will generate inline SEH automatically
}



/* Function start: 0x424b00 */ /* DEMO ONLY - no full game match */
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
