/**
 * @file arr.c
 * @author Mathieu Fourcroy
 * @date 07/15
 * @brief Contains functions for handling array which are widely use in this
 *  program.
 */

/*=====| INCLUDES |===========================================================*/
#include "arr.h"

/*=====| FUNCTIONS |==========================================================*/
/** Subtract a given float to every elements of a float array.
 *
 * @param[out] dst  The destination array which will contains the resulting  
 *  values. It must be of same size as 'src'.
 * @param[in]  src  The source array from which the results will be computed.
 * @param[in]  size The size of the 'dst' and 'src' arrays.
 * @param[in]  f    The float number which will be subtract to every elements of
 *  the 'src' array.
 */
void arr_sub(float *dst, float *src, size_t size, float f){
    size_t i;

    for(i = 0; i < size; i++){
        dst[i] = pow(src[i] - f, 2);
    }
}

/** Compute the sum of two arrays.
 *
 * @warning The content of the 'dst' array will be modified to contains
 * 'dst' + 'src'..
 *
 * @note The two arrays should have the same size which is 'size'.
 *
 * @param[in,out] dst  The first array of the sum. It will contains the result.
 * @param[in]     src  The second array of the sum.
 * @param[in]     size The size of the arrays.
 */
void arr_add(float dst[], float src[], size_t size){
    size_t i;

    for(i = 0; i < size; i++){
        dst[i] = dst[i] + src[i];
    }
}

/** Compute the absolute value of every elements of a given array.
 *
 * @note The two arrays should have the same size which is 'size'.
 *
 * @param[out] dst  The array which will contains the result: the absolute 
 *  values of 'src' elements..
 * @param[in]  src  The initial array.
 * @param[in]  size The size of the arrays.
 */
void arr_abs(float *dst, float *src, size_t size){
    size_t i;

    for(i = 0; i < size; i++){
        dst[i] = fabs(src[i]);
    }
}

/** Compute and returns the sum of every elements of a given array.
 *
 * @param[in] arr  The array to sum.
 * @param[in] size The size of the arrays.
 *
 * @return The sum of every elements of the given array of size 'size'.
 */
float arr_sum(float *arr, size_t size){
    size_t i;
    float res = 0.;

    for(i = 0; i < size; i++){
        res = res + arr[i];
    }
    return res;
}

/** Find and returns the index of the minimum value in the given array.
 *
 * @param[in] arr The array.
 * @param[in] size The array size.
 *
 * @return The index of the lowest value of the array 'arr'.
 */
size_t arr_min_idx(const float *arr, size_t size){
    size_t i;
    float minimum = arr[0];
    size_t idx = 0;

    for (i = 1; i < size; ++i){
        if (minimum > arr[i]){
            minimum = arr[i];
            idx = i;
        }
    }
    return idx;
}

/** Flatten a 2D array and modify the given image data.
 *
 * @param[in,out] img      The image wich will be modified with the data of 
 *  'arr'.
 * @param[in]     arr      The 2D array containing the posterized pixels.
 * @param[in]     height   The image height.
 * @param[in]     width    The image width.
 * @param[in]     channels The number of channels of the image.
 */
void arr_to_IplImage(IplImage *img, float **arr, int height, int width,
                     int channels){
    int i, j;

    for(i = 0; i < width; i++){
        for(j = 0; j < height; j++){
            CV_IMAGE_ELEM(img, uchar, j, i * 3 + 2) =\
                (uchar)(arr[i * height + j][0]);
            CV_IMAGE_ELEM(img, uchar, j, i * 3 + 1) =\
                (uchar)(arr[i * height + j][1]);
            CV_IMAGE_ELEM(img, uchar, j, i * 3 + 0) =\
                (uchar)(arr[i * height + j][2]);
        }
    }
}
