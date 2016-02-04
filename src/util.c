/*====| INCLUDES |============================================================*/
#include <stdlib.h>
#include "util.h"
#include "arr.h"

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

const char *get_filename_ext(const char *filename){
    const char *dot = strrchr(filename, '.');

    if(!dot || dot == filename) return "";
    return dot + 1;
}

/** Compute the Euclidian distance between two points
 *
 * The Euclidean distance between points p and q is the length of the line 
 * segment connecting them.
 *
 * @see https://en.wikipedia.org/wiki/Euclidean_distance.
 *
 * The two points here are (x, y, z) and (rgb[0], rgb[1], rgb[2]).
 *
 * @param[out] res  The array which will contain the euclidian distance. It must
 *  be of size 'size'.
 * @param[in]  size Size of the 'x', 'y', 'z' and 'res' arrays.
 * @param[in]  x    The x 3D coordinates.
 * @param[in]  y    The y 3D coordinates.
 * @param[in]  z    The z 3D coordinates.
 * @param[in]  rgb  The array containing the three channels values, treated as
 *  coordinates.
 */
void euclidian(float *res, size_t size, float *x, float *y, float *z,
               float *rgb){
    float *xsub = malloc(sizeof(float) * size);
    float *ysub = malloc(sizeof(float) * size);
    float *zsub = malloc(sizeof(float) * size);
    float *sum = malloc(sizeof(float) * size);
    size_t i;

    memset(xsub, 0, size);
    memset(ysub, 0, size);
    memset(zsub, 0, size);

    arr_sub(xsub, x, size, rgb[0]);
    arr_sub(ysub, y, size, rgb[1]);
    arr_sub(zsub, z, size, rgb[2]);

    for(i = 0; i < size; i++){
        sum[i] = xsub[i] + ysub[i] + zsub[i];
    }

    for(i = 0; i < size; i++){
        float r = (float)sqrt(sum[i]);
        //~ res[i] =  sqrt(sum[i]);
        memcpy(&res[i], &r, sizeof(float));
    }

    free(sum);
    free(xsub);
    free(ysub);
    free(zsub);
}
