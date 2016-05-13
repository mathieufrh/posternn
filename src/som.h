#ifndef _SOM_H_
#define _SOM_H_

/*====| INCLUDES |============================================================*/
#include <stdlib.h>

/*====| DEFINES |=============================================================*/
#define SOM_NO_MEMORY 10
#define SOM_OK 0

/*====| PROTOTYPES |==========================================================*/
void compute_delta(float *res, float eta, float *neigh, size_t nbNeigh, 
                   float chan, float *chanArr);
int som_train(float **res, float **imgPixels, unsigned int nbPixels,
              int nbNeurons, int noEpoch, float tresh);
void som_posterize(float **postPixels, float **origPixels, float *train[],
                   unsigned int nbPixels, int nbNeurons);
#endif
