/**
 * @file util.c
 * @author Mathieu Fourcroy
 * @date 07/15
 * @brief Contains helper functions use in any other source file.
 */

/*====| INCLUDES |============================================================*/
#include <stdlib.h>
#include "util.h"
#include "arr.h"

/*=====| FUNCTIONS |==========================================================*/
/** Fill the given array with float numbers between 0 and 1.
 *
 * @note You must run srand() before in order to initialize the random number
 *  generation seed.
 * @note The array 'arr' size must be 'size'.
 *
 * @param[out] arr  The array to fill.
 * @param[in]  size The number of value to add to the array.
 */
void random_sample(float *arr, size_t size){
    size_t i;

    for(i = 0; i < size; i++){
        arr[i] = (float)rand() / (float)(RAND_MAX / 1);
    }
}

/** Returns a randomly generated integer between 0 and the given maximum.
 *
 * @param[in] max The maximum boundary.
 *
 * @return The randomly generated number.
 */
unsigned int random_uint (unsigned int max){
    return (unsigned int)rand() % max;
}

/** Determine the extension (format) of a file.
 *
 * @param[in] filename The file name or its full path.
 *
 * @return The file extension (for instance jpg or png).
 */
const char *get_filename_ext(const char *filename){
    const char *dot = strrchr(filename, '.');

    if(!dot || dot == filename) return "";
    return dot + 1;
}
