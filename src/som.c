/**
 * @file som.c
 * @author Mathieu Fourcroy
 * @date 07/15
 * @brief Contains functions about the self organized map neural network 
 *  implemented for the project.
 */

/*=====| INCLUDES |===========================================================*/
#include <string.h>
#include <time.h>
#include <math.h>
#include <limits.h>
#include "som.h"
#include "arr.h"
#include "util.h"

/*=====| FUNCTIONS |==========================================================*/
/** Compute and returns the neighbour radius value.
 *
 * The radius is computed given the current iteration number, the maximum 
 * iteration number and the dimmensions of the network.
 *
 * @param[in] iterNo    The number of the current iteration.
 * @param[in] iterCount The maximum number of iterations.
 * @param[in] width     The width of the network.
 * @param[in] height     The height of the network.
 *
 * @return The computed radius.
 */
static float som_radius(int iterNo, int iterCount, int width, int height){
    float totalrange = max(width, height) / 2.;
    float step = iterNo / (float)iterCount;

    // Quadratic function (**2 can be changed for different function)
    return totalrange - pow(step, 2) * totalrange;
}

/** Compute and returns the learning rate of the network.
 *
 * The learning rate is a function of the current iteration number over the 
 * maximum iterations number and a given range.
 *
 * @param[in] iterNo    The number of the current iteration.
 * @param[in] iterCount The maximum number of iterations.
 *
 * @return The computed learning rate.
 */
static float som_learning_rate(int iterNo, int iterCount){
    float MAX_VALUE = 0.75;
    float MIN_VALUE = 0.1;
    float totalrange = MAX_VALUE - MIN_VALUE;
    float step = iterNo / (float)iterCount;

    // Linear function
    return MAX_VALUE - step * totalrange;
}

/** Compute the Euclidian distance between weight vectors and input vector.
 *
 * The Euclidean distance between points p and q is the length of the line 
 * segment connecting them.
 *
 * @see https://en.wikipedia.org/wiki/Euclidean_distance.
 *
 * The two points here are (x, y, z) and (RGB[0], RGB[1], RGB[2]).
 *
 * @param[out] res  The array which will contain the euclidian distance. It must
 *  be of size 'size'.
 * @param[in]  size Size of the 'x', 'y', 'z' and 'res' arrays.
 * @param[in]  x    The x 3D coordinates.
 * @param[in]  y    The y 3D coordinates.
 * @param[in]  z    The z 3D coordinates.
 * @param[in]  RGB  The array containing the three channels values, treated as
 *  coordinates.
 *
 * @return SOM_OK if everything goes right or SOM_NO_MEMORY if a memory 
 *  allocation (malloc) fail.
 */
static int euclidian(float *res, size_t size, float *x, float *y, float *z,
                     float *RGB){
    float *xsub = malloc(sizeof(float) * size);
	if(xsub == NULL){
		return SOM_NO_MEMORY;
    }
    float *ysub = malloc(sizeof(float) * size);
	if(ysub == NULL){
		return SOM_NO_MEMORY;
    }
    float *zsub = malloc(sizeof(float) * size);
	if(zsub == NULL){
		return SOM_NO_MEMORY;
    }
    float *sum = malloc(sizeof(float) * size);
	if(sum == NULL){
		return SOM_NO_MEMORY;
    }
    float r;
    size_t i;

    memset(xsub, 0, size);
    memset(ysub, 0, size);
    memset(zsub, 0, size);

    arr_sub(xsub, x, size, RGB[0]);
    arr_sub(ysub, y, size, RGB[1]);
    arr_sub(zsub, z, size, RGB[2]);

    for(i = 0; i < size; i++){
        sum[i] = xsub[i] + ysub[i] + zsub[i];
        r = (float)sqrt(sum[i]);
        memcpy(&res[i], &r, sizeof(float));
    }

    free(sum);
    free(xsub);
    free(ysub);
    free(zsub);

    return SOM_OK;
}

/** Compute the euclidian distance between two 2D points.
 *
 * @see https://en.wikipedia.org/wiki/Euclidean_distance
 *
 * @param[in] i The first point abscissae.
 * @param[in] y The first point ordinates.
 * @param[in] j The second point abscissae.
 * @param[in] x The second point ordinates.
 *
 * @return The euclidian distance between the points (i, j) and (y, x).
 */
static float compute_distance(int i, int y, int j, int x){
    return sqrt(pow(i - y, 2) + pow(j - x, 2));
}

/** Compute the neighbors of a given point which are in a given radius.
 *
 * Generates a mask with [0, 1] values to activate the inside of the 'radius' 
 * radius circle centered in (x, y).
 *
 * @param[out] neigh  The resulting mask array.
 * @param[in]  x      The abscissae of the radius center.
 * @param[in]  y      The ordinates of the radius center.
 * @param[in]  radius The neighbooring radius.
 * @param[in]  width  The width of the network.
 * @param[in]  height The height of the network.
 */
static void som_neighbourhood(float *neigh, int x, int y, float radius,
                              int width, int height){
    int i, j, k = 0;
    float distance;

    for(i = 0; i < height ; i++){
        for(j = 0; j < width ; j++){
            distance = compute_distance(i, y, j, x);
            if(distance <= radius){
                neigh[k] = 1 - distance / (float)radius;
            }
            else{
                neigh[k] = 0;
            }
            k++;
        }
    }
}

/** Compute the new neurons values (learning stage).
 *
 * This function compute the new neurons values depending on the learning rate
 * (eta), the winner neighboors (neigh) and the choosed input vector (chan).
 * The network wieght vectors inside the neighbooring radius are modified so 
 * that they will be more similar to the choosen input vector.
 *
 * @param[out] res     The resulting array containing the delta values.
 * @param[in]  eta     The learning rate.
 * @param[in]  neigh   Array listing the current neighbors of the network.
 * @param[in]  nbNeigh The size of the "neigh" array.
 * @param[in]  chan    The value of the color channel.
 * @param[in]  chanArr The array corresponding to the given channel.
 */
void compute_delta(float *res, float eta, float *neigh, size_t nbNeigh, 
                   float chan, float *chanArr){
    size_t i;

    for(i = 0; i < nbNeigh; i++){
        res[i] = eta * neigh[i] * (chan - chanArr[i]);
    }
}

/** Train the unsupervised SOM network.
 *
 * The network is initialized with random (non-graduate) values. It is then
 * trained with the image pixels (RGB). Once the network is done training the
 * centroids of the resulting clusters is returned.
 *
 * @note The length of the clusters depends on the parameter 'n'. The greatest
 *  n, the more colors in the clusters, the less posterized the image.
 *
 * @param[out] res       The resulting R, G and B clusters centroids.
 * @param[in]  imgPixels The original image pixels.
 * @param[in]  nbPixels  The number of pixels of th image (height * width).
 * @param[in]  nbNeurons The posterization leveldefined by its number of 
 *                       neurons.
 * @param[in]  noEpoch   Number of training passes (a too small number of epochs
 *  won't be enough for the network to correctly know the image but a too high
 *  value will force the network to modify the wieghts so much that the image
 *  can actually looks "ugly").
 * @param[in]  thresh    The threshold value. The SOM delta parameter and the
 *  training rate are dicreasing from one epoch to the next. The treshold value
 *  set a stop condifition in case the value has fallen under this minimum 
 *  value.
 *
 * @return SOM_OK if everything goes right or SOM_NO_MEMORY if a memory 
 *  allocation (malloc) fail.
 */
int som_train(float **res, float **imgPixels, unsigned int nbPixels,
              int nbNeurons, int noEpoch, float thresh){
    int mapWidth =              // Map width. This can be calculated from its
        (int)sqrt(nbNeurons);   // number of neurons as the map is square.
    int mapHeight =             // Map height. This can be calculated from its
        (int)sqrt(nbNeurons);   // number of neurons as the map is square.
    float delta = INT_MAX;      // Start with an almost impossible value
    int it = 0;                 // Count the network iterations
    unsigned int pick;          // The choosen input pixel
    float rad;                  // Neighbooring radius (keep dicreasing)
    float eta;                  // Learning rate (keep dicreasing)
    float pickRGB[3] = {0};     // Contains the choosen input RGB vector
    size_t choosen;             // Best Matching Unit (BMU)
    int choosen_x;              // BMU abscissa
    int choosen_y;              // BMU ordinate
    float *neigh =              // Neighbooring mask
        malloc(sizeof(float) * nbNeurons);
	if(neigh == NULL){
		return SOM_NO_MEMORY;
    }
    memset(neigh, 0, nbNeurons);

    float *WR =                 // RED part of the network weight vectors
        malloc(sizeof(float) * nbNeurons);
	if(WR == NULL){
		return SOM_NO_MEMORY;
    }
    memset(WR, 0, nbNeurons);
    float *WG =                 // GREEN part of the network weight vectors
        malloc(sizeof(float) * nbNeurons);
	if(WG == NULL){
		return SOM_NO_MEMORY;
    }
    memset(WG, 0, nbNeurons);
    float *WB =                 // BLUE part of the network weight vectors
        malloc(sizeof(float) * nbNeurons);
	if(WB == NULL){
		return SOM_NO_MEMORY;
    }
    memset(WB, 0, nbNeurons);

    float *deltaR =             // New RED part of the network weight vectors
        malloc(sizeof(float) * nbNeurons);
	if(deltaR == NULL){
		return SOM_NO_MEMORY;
    }
    memset(deltaR, 0, nbNeurons);
    float *deltaG =             // New GREEN part of the network weight vectors
        malloc(sizeof(float) * nbNeurons);
	if(deltaG == NULL){
		return SOM_NO_MEMORY;
    }
    memset(deltaG, 0, nbNeurons);
    float *deltaB =             // New BLUE part of the network weight vectors
        malloc(sizeof(float) * nbNeurons);
	if(deltaB == NULL){
		return SOM_NO_MEMORY;
    }
    memset(deltaB, 0, nbNeurons);

    float *absDeltaR =          // Absolute value of deltaR
        malloc(sizeof(float) * nbNeurons);
	if(absDeltaR == NULL){
		return SOM_NO_MEMORY;
    }
    memset(absDeltaR, 0, nbNeurons);
    float *absDeltaG =          // Absolute value of deltaG
        malloc(sizeof(float) * nbNeurons);
	if(absDeltaG == NULL){
		return SOM_NO_MEMORY;
    }
    memset(absDeltaG, 0, nbNeurons);
    float *absDeltaB =          // Absolute value of deltaB
        malloc(sizeof(float) * nbNeurons);
	if(absDeltaB == NULL){
		return SOM_NO_MEMORY;
    }
    memset(absDeltaB, 0, nbNeurons);

    float *dists =              // Vectors euclidian distances from input form
        malloc(sizeof(float) * nbNeurons);
	if(dists == NULL){
		return SOM_NO_MEMORY;
    }
    memset(dists, 0, nbNeurons);

    /* Randomly initialize weight vectors */
    srand(time(NULL));
    random_sample(WR, nbNeurons);
    random_sample(WG, nbNeurons);
    random_sample(WB, nbNeurons);

    while(it < noEpoch && delta >= thresh){
        /* Randomly choose an input form */
        pick = random_uint(nbPixels);
        pickRGB[0] = imgPixels[pick][0];
        pickRGB[1] = imgPixels[pick][1];
        pickRGB[2] = imgPixels[pick][2];

        /* Compute every vectors euclidian distance from the input form */
        euclidian(dists, nbNeurons, WR, WG, WB, pickRGB);
        /* Determine the BMU */
        choosen = arr_min_idx(dists, nbNeurons);
        choosen_x = (int)choosen % mapWidth;
        choosen_y = choosen / mapHeight;

        /* Compute the new neighbooring radius */
        rad = som_radius(it, noEpoch, mapWidth, mapHeight);
        /* Find the BMU neighboors */
        som_neighbourhood(neigh, choosen_x, choosen_y, rad, mapWidth, 
                          mapHeight);

        /* Compute the new learning rate */
        eta = som_learning_rate(it, noEpoch);
        /* Compute new value of the network weight vectors */
        compute_delta(deltaR, eta, neigh, nbNeurons, pickRGB[0], WR);
        compute_delta(deltaG, eta, neigh, nbNeurons, pickRGB[1], WG);
        compute_delta(deltaB, eta, neigh, nbNeurons, pickRGB[2], WB);
        /* Update the network weight vectors values */
        arr_add(WR, deltaR, nbNeurons);
        arr_add(WG, deltaG, nbNeurons);
        arr_add(WB, deltaB, nbNeurons);

        arr_abs(absDeltaR, deltaR, nbNeurons);
        arr_abs(absDeltaG, deltaG, nbNeurons);
        arr_abs(absDeltaB, deltaB, nbNeurons);
        delta = arr_sum(absDeltaR, nbNeurons) +
                arr_sum(absDeltaG, nbNeurons) +
                arr_sum(absDeltaB, nbNeurons);

        it++;
    }
    res[0] = WR;
    res[1] = WG;
    res[2] = WB;

    free(dists);
    free(deltaR);
    free(deltaG);
    free(deltaB);
    free(absDeltaR);
    free(absDeltaG);
    free(absDeltaB);
    free(neigh);

    return SOM_OK;
}

/** Posterize an image from the trained SOM otput.
 *
 * This function fill a vector containing the RGB values of each pixels of an
 * image with the output of a trained SOM. The original image pixels RGB values
 * are needed to compute the euclidian distance from its colors to the trained
 * SIOM output colors.
 * Basicaly you can see this function job as a smart color selector. For each
 * pixel of the original image the function compute the "nearest" color among
 * the reduced set of olors of the SOM output.
 *
 * @param[out] postPixels The 2D array wich will contains the posterized pixels.
 * @param[in]  origPixels The original image pixels.
 * @param[in]  train      The trained SOM output vector (its map).
 * @param[in]  nbPixels   The number of pixels of th image (height * width).
 * @param[in]  nbNeurons  The posterization level defined by its number of 
 *                        neurons.
 */
void som_posterize(float **postPixels, float **origPixels, float *train[],
                   unsigned int nbPixels, int nbNeurons){
    unsigned int i;
    float dists[nbNeurons];
    float choosen, o0, o1, o2;

    for(i = 0; i < nbPixels; i++){
        euclidian(dists, nbNeurons, train[0], train[1], train[2], 
                  origPixels[i]);
        choosen = arr_min_idx(dists, nbNeurons);

        o0 = train[0][(int)choosen];
        o1 = train[1][(int)choosen];
        o2 = train[2][(int)choosen];

        postPixels[i][0] = (int)(o0 * 255.);
        postPixels[i][1] = (int)(o1 * 255.);
        postPixels[i][2] = (int)(o2 * 255.);
    }
}
