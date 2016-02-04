#include <opencv/highgui.h>
#include <stdio.h>
#include <ctype.h>
#include <getopt.h>
#include "arr.h"
#include "som.h"
#include "util.h"

void usage(void){
    printf("USAGE: som [-i input_file] [-l posterization_level]\n"\
           "           [-o output_file]\n");
}

int set_vars_from_args(int argc, char * const argv[], int *postLevel,
                       char *inFile, char *outFile){
    extern char *optarg;
    int index;
    int tmp;
    int c;
    int res = 0;

    opterr = 0;
    while((c = getopt(argc, argv, "i:l:o:")) != -1){
        switch(c){
            case 'i':
                strcpy(inFile, optarg);
                break;
            case 'l':
                tmp = (int)strtol(optarg, NULL, 10);
                if(tmp > 0){
                    *postLevel = tmp;
                }
                else{
                    fprintf(stderr, "WARNING: Invalid argument for option -l. "\
                            "Using default value.\n");
                }
                break;
            case 'o':
                strcpy(outFile, optarg);
                break;
            case '?':
                if(optopt == 'c'){
                    fprintf(stderr, "Option -%c requires an argument.\n",
                            optopt);
                }
                else if(isprint(optopt)){
                    fprintf(stderr, "Unknown option `-%c'.\n", optopt);
                }
                else{
                    fprintf(stderr, "Unknown option character `\\x%x'.\n",
                            optopt);
                }
                return EXIT_FAILURE;
            default:
                abort();
        }
    }
    for(index = optind; index < argc; index++)
        printf("Non-option argument: %s\n", argv[index]);
    if(strcmp(inFile, "") == 0){
        fprintf(stderr, "ERROR: input file is missing\n");
        usage();
        res = 1;
    }
    return res;
}

int main(int argc, char * const argv[]){
    const char *ext;
    char saveName[PATH_MAX];
    unsigned int nbPixels;
	float **origPixels, **segmPixels, **trainRes;
    int height;
    int width;
    int step;
    int channels;
    int i;
    int j;
    int postLevel = 2;
    char inFile[PATH_MAX] = {'\0'};
    char outFile[PATH_MAX] = {'\0'};

    /* Set the program variables using the cmdl arguments */
    if(set_vars_from_args(argc, argv, &postLevel, inFile, outFile) > 0){
        return EXIT_FAILURE;
    }
    ext = get_filename_ext(inFile);

    /* Load the image */
    IplImage *img = cvLoadImage(inFile, CV_LOAD_IMAGE_COLOR);
    if(!img){
        printf("Image can NOT Load!!!\n");
        return 1;
    }

    /* Get the image data */
    height = img->height;
    width = img->width;
    step = img->widthStep;
    channels = img->nChannels;
    nbPixels = height * width;

    /* Alloc everything */
	origPixels = malloc(nbPixels * sizeof(float *));
	if(origPixels == NULL){
		fprintf(stderr, "out of memory\n");
		return EXIT_FAILURE;
    }
	for(i = 0; i < nbPixels; i++){
		origPixels[i] = malloc(channels * sizeof(float));
		if(origPixels[i] == NULL){
			fprintf(stderr, "out of memory\n");
			return EXIT_FAILURE;
        }
    }
    for(i = 0; i < width; i++){
        for(j = 0; j < height; j++){
            uchar blue = CV_IMAGE_ELEM( img, uchar, j, i * 3 + 0);
            uchar green = CV_IMAGE_ELEM( img, uchar, j, i * 3 + 1);
            uchar red = CV_IMAGE_ELEM( img, uchar, j, i * 3 + 2);
            origPixels[i * height + j][0] = red / 255.;
            origPixels[i * height + j][1] = green / 255.;
            origPixels[i * height + j][2] = blue / 255.;
        }
    }
	segmPixels = malloc(nbPixels * sizeof(float *));
	if(segmPixels == NULL){
		fprintf(stderr, "out of memory\n");
		return EXIT_FAILURE;
    }
	for(i = 0; i < nbPixels; i++){
		segmPixels[i] = malloc(channels * sizeof(float));
		if(segmPixels[i] == NULL){
			fprintf(stderr, "out of memory\n");
			return EXIT_FAILURE;
        }
    }
    for(i = 0; i < width; i++){
        for(j = 0; j < height; j++){
            uchar blue = CV_IMAGE_ELEM( img, uchar, j, i * 3 + 0);
            uchar green = CV_IMAGE_ELEM( img, uchar, j, i * 3 + 1);
            uchar red = CV_IMAGE_ELEM( img, uchar, j, i * 3 + 2);
            segmPixels[i * height + j][0] = red / 255.;
            segmPixels[i * height + j][1] = green / 255.;
            segmPixels[i * height + j][2] = blue / 255.;
        }
    }
    trainRes = malloc(channels * sizeof(float *));
    for(i = 0; i < channels; i++){
        trainRes[i] = malloc(sizeof(float) * (int)pow(postLevel, 2));
    }

    som_train((float **)trainRes, origPixels, nbPixels, postLevel);
    som_segmentation(segmPixels, origPixels, trainRes, nbPixels, postLevel);
    arr_flatten(img, segmPixels, height, width, channels, step);
    cvNamedWindow("myfirstwindow", CV_WINDOW_AUTOSIZE);
    cvShowImage("myfirstwindow", img);
    cvWaitKey(0);
    if(strcmp(outFile, "") != 0){
        cvSaveImage(outFile, img, 0);
    }
    else{
        strcpy(saveName, inFile);
        saveName[strlen(saveName) - strlen(ext) - 1] = '\0';
        strcat(saveName, "_posterized.");
        strcat(saveName, ext);
        cvSaveImage(saveName, img, 0);
    }
    
    /* Free everything */
	for(i = 0; i < nbPixels; i++){
        free(origPixels[i]);
    }
    free(origPixels);
	for(i = 0; i < nbPixels; i++){
        free(segmPixels[i]);
    }
    free(segmPixels);
    for(i = 0; i < channels; i++){
        free(trainRes[i]);
    }
    free(trainRes);
    cvReleaseImage(&img);
    cvReleaseImageHeader(&img);
    cvDestroyWindow("myfirstwindow");

    return 0;
}
