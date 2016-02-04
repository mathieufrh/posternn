#include <string.h>
#include <time.h>
#include <math.h>
#include "som.h"
#include "arr.h"
#include "util.h"

/** Compute and returns the radius value.
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
float radius(int iterNo, int iterCount, int width, int height){
    float MAX_VALUE = max(width, height) / 2.;
    float MIN_VALUE = 0.;
    float totalrange = MAX_VALUE - MIN_VALUE;
    float step = iterNo / (float)iterCount;

    // Quadratic function (**2 can be changed for different function)
    float r = MAX_VALUE - pow(step, 2) * totalrange;
    return r;
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
float learning_rate(int iterNo, int iterCount){
    float MAX_VALUE = 0.75;
    float MIN_VALUE = 0.1;
    float totalrange = MAX_VALUE - MIN_VALUE;
    float step = iterNo / (float)iterCount;

    // Liniar function
    float lr = MAX_VALUE - pow(step, 1) * totalrange;
    return lr;
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
float compute_distance(int i, int y, int j, int x){
    return sqrt(pow(i - y, 2) + pow(j - x, 2));
}

/** Compute the neighborsof a given point which are in a given radius.
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
void neighbourhood(float *neigh, int x, int y, float radius, int width,
                   int height){
    int i, j, k = 0;
    float distance;
    float mask[height][width];

    for(i = 0; i < height ; i++){
        for(j = 0; j < width ; j++){
            distance = compute_distance(i, y, j, x);
            if(distance <= radius){
                mask[i][j] = 1 - distance / (float)radius;
            }
            else{
                mask[i][j] = 0;
            }
        }
    }
    for(i = 0; i < height ; i++){
        for(j = 0; j < width ; j++){
            neigh[k] = mask[i][j];
            k++;
        }
    }
}

/** Compute the neuron delta.
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
    float resNeigh[nbNeigh];
    float resChanArr[nbNeigh];

    for(i = 0; i < nbNeigh; i++){
        resNeigh[i] = eta * neigh[i];
    }
    for(i = 0; i < nbNeigh; i++){
        resChanArr[i] = chan - chanArr[i];
    }
    for(i = 0; i < nbNeigh; i++){
        res[i] = resNeigh[i] * resChanArr[i];
    }
}

void som_train(float **res, float **imgPixels, unsigned int imgLen, int n){
    int maxIters = 3000;
    float thresh = 0.001;
    unsigned int pick;
    float rad;
    float eta;
    float pick_rgb[3] = {0};
    float choosen;
    int choosen_x;
    int choosen_y;
    float *neigh = malloc(sizeof(float) * (int)pow(n, 2));
    memset(neigh, 0, (int)pow(n, 2));

    // Randomly init neurons' color
    float *WR = malloc(sizeof(float) * (int)pow(n, 2));
    memset(WR, 0, (int)pow(n, 2));
    float *WG = malloc(sizeof(float) * (int)pow(n, 2));
    memset(WG, 0, (int)pow(n, 2));
    float *WB = malloc(sizeof(float) * (int)pow(n, 2));
    memset(WB, 0, (int)pow(n, 2));

    float *deltaR = malloc(sizeof(float) * (int)pow(n, 2));
    memset(deltaR, 0, (int)pow(n, 2));
    float *deltaG = malloc(sizeof(float) * (int)pow(n, 2));
    memset(deltaG, 0, (int)pow(n, 2));
    float *deltaB = malloc(sizeof(float) * (int)pow(n, 2));
    memset(deltaB, 0, (int)pow(n, 2));

    float *absDeltaR = malloc(sizeof(float) * (int)pow(n, 2));
    memset(absDeltaR, 0, (int)pow(n, 2));
    float *absDeltaG = malloc(sizeof(float) * (int)pow(n, 2));
    memset(absDeltaG, 0, (int)pow(n, 2));
    float *absDeltaB = malloc(sizeof(float) * (int)pow(n, 2));
    memset(absDeltaB, 0, (int)pow(n, 2));

    srand(time(NULL));
    random_sample(WR, (size_t)pow(n, 2));
    random_sample(WG, (size_t)pow(n, 2));
    random_sample(WB, (size_t)pow(n, 2));

    float delta = 99999;
    int it = 0;
    float *dists = malloc(sizeof(float) * (int)pow(n, 2));
    memset(dists, 0, (int)pow(n, 2));

    while(it < maxIters && delta >= thresh){
        pick = random_uint(imgLen);
        pick_rgb[0] = imgPixels[pick][0];
        pick_rgb[1] = imgPixels[pick][1];
        pick_rgb[2] = imgPixels[pick][2];

        // Computes all distances and picks the closest
        euclidian(dists, (size_t)pow(n, 2), WR, WG, WB, pick_rgb);
        choosen = arr_min_idx(dists, (int)pow(n, 2));

        // Restore matrix indeces from liniar index
        choosen_x = (int)choosen % n;
        choosen_y = choosen / n;

        rad = radius(it, maxIters, n, n);
        eta = learning_rate(it, maxIters);
        // Generate neighbouring mask used to only influence the neighbours
        neighbourhood(neigh, choosen_x, choosen_y, rad, n, n);

        // Compute deltas
        compute_delta(deltaR, eta, neigh, (int)pow(n, 2), pick_rgb[0], WR);
        compute_delta(deltaG, eta, neigh, (int)pow(n, 2), pick_rgb[1], WG);
        compute_delta(deltaB, eta, neigh, (int)pow(n, 2), pick_rgb[2], WB);

        // Update weights
        arr_add(WR, deltaR, (int)pow(n, 2));
        arr_add(WG, deltaG, (int)pow(n, 2));
        arr_add(WB, deltaB, (int)pow(n, 2));

        // Convergence check
        arr_abs(absDeltaR, deltaR, (size_t)pow(n, 2));
        arr_abs(absDeltaG, deltaG, (size_t)pow(n, 2));
        arr_abs(absDeltaB, deltaB, (size_t)pow(n, 2));
        delta = arr_sum(absDeltaR, (size_t)pow(n, 2)) + arr_sum(absDeltaG, (size_t)pow(n, 2)) + arr_sum(absDeltaB, (size_t)pow(n, 2));

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
}

void som_segmentation(float **segmPixels, float **origPixels, float *train[],
                      unsigned int nbPixels, int n){
    unsigned int i;
    float dists[(int)pow(n, 2)];
    float choosen, o0, o1, o2;

    for(i = 0; i < nbPixels; i++){
        euclidian(dists, (size_t)pow(n, 2), train[0], train[1], train[2], origPixels[i]);
        choosen = arr_min_idx(dists, (int)pow(n, 2));

        o0 = train[0][(int)choosen];
        o1 = train[1][(int)choosen];
        o2 = train[2][(int)choosen];

        segmPixels[i][0] = (int)(o0 * 255.);
        segmPixels[i][1] = (int)(o1 * 255.);
        segmPixels[i][2] = (int)(o2 * 255.);
    }
}
