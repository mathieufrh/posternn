#ifndef _SOM_H_
#define _SOM_H_

/*====| INCLUDES |============================================================*/
#include <stdlib.h>

/*====| PROTOTYPES |==========================================================*/
float radius(int iterNo, int iterCount, int width, int height);
float learning_rate(int iterNo, int iterCount);
float compute_distance(int i, int y, int j, int x);
void neighbourhood(float *neigh, int x, int y, float radius, int width,
                   int height);
void compute_delta(float *res, float eta, float *neigh, size_t nbNeigh, 
                   float chan, float *chanArr);
void som_train(float **res, float **imgPixels, unsigned int imgLen, int n);
void som_segmentation(float **segmPixels, float **origPixels, float *train[],
                      unsigned int nbPixels, int n);
#endif
