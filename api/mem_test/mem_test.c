#include <stdio.h>
#include <stdlib.h>
#include <memory.h>

#define BLOCK_SIZE (1024 * 1024)

int main(void)
{

    for (unsigned index = 0;; index++) {

        unsigned *array = malloc(BLOCK_SIZE);
        if (array == NULL) {
            fprintf(stderr, "Not enough memory\n");
            sleep(-1);
            exit(1);
        }
        fprintf(stderr, "%2u: Memory allocated in Byte: %u\n", index, BLOCK_SIZE);
        for (unsigned k = 0; k < BLOCK_SIZE / sizeof(unsigned); k++) {
            array[k] = k;
        }
        array[100] = 99;
        for (unsigned k = 0; k < BLOCK_SIZE / sizeof(unsigned); k++) {
            if (array[k] != k) {
                fprintf(stderr, "Oops! value at %zd should be %zd but %u\n", k, k, array[k]);
            }
        }

    }
}
