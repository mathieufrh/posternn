#ifndef _ARR_H_
#define _ARR_H_

/*====| INCLUDES |============================================================*/
#include <opencv/cv.h>

/*====| PROTOTYPES |==========================================================*/
void arr_sub(float *dst, float *src, size_t size, float f);
void arr_add(float dst[], float src[], size_t size);
void arr_abs(float *dst, float *src, size_t size);
float arr_sum(float *arr, size_t size);
size_t arr_min_idx(const float *arr, size_t size);
void arr_to_IplImage(IplImage *img, float **arr, int height, int width,
                     int channels);

#endif