#ifndef _UTIL_H_
#define _UTIL_H_

/*====| INCLUDES |============================================================*/
#include <string.h>

/*====| DEFINES |=============================================================*/
#define max(a,b) \
        ({ __typeof__ (a) _a = (a); \
        __typeof__ (b) _b = (b); \
        _a > _b ? _a : _b; })

/*====| PROTOTYPES |==========================================================*/
void random_sample(float *arr, size_t size);
unsigned int random_uint (unsigned int max);
const char *get_filename_ext(const char *filename);

#endif