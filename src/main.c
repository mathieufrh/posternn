/**
 * @file main.c
 * @author Mathieu Fourcroy
 * @date 07/15
 * @brief Main file of the program. Take care of I/O and call the other
 *  functions.
 */

/*=====| INCLUDES |===========================================================*/
#include <opencv/highgui.h>
#include <stdio.h>
#include <ctype.h>
#include <getopt.h>
#include "arr.h"
#include "som.h"
#include "util.h"

/*=====| FUNCTIONS |==========================================================*/
/** Print a usage message.
 */
void usage(void){
    printf("USAGE: som -i input_file [-l posterization_level]\n"\
           "           [-e number8of8epochs] [-t treshold]\n"\
           "           [-o output_file]\n\n"\
           "       options description:\n"\
           "           -i Specify the input image to posterize.\n"\
           "           -l Specify the posterization level.\n"\
           "              The higher the level, the more colors.\n"\
           "           -e Specify the number of iterations of the SOM.\n"\
           "           -t Specify the network threshold value.\n"\
           "              If the network delta value ever fall under\n"\
           "              this threshold, the training stop.\n"\
           "           -o Specify the output posterized image path.\n");
}

/** Parse the options from the command line.
 *
 * @param[in]  argc       Number of arguments on the command line.
 * @param[in]  argv       The arguments of the command line.
 * @param[out] postLevel  Posterization level (set by -l / default: 2).
 * @param[out] epochs     Number of training stage (set by -e / default: 3000).
 * @param[out] thresh     Network threshold value (set by -t / default: 0.001).
 * @param[out] inFile     Path to the input image (must be set with -i).
 * @param[out] outFile    Path to the output image (set by -o).
 */
int set_vars_from_args(int argc, char * const argv[], int *postLevel,
                       int *epochs, float *thresh, char *inFile, char *outFile){
    extern char *optarg;
    int index;
    int tmp;
    float ftmp;
    int c;
    int res = 0;

    opterr = 0;
    while((c = getopt(argc, argv, "hi:l:e:t:o:")) != -1){
        switch(c){
            case 'h':
                printf(
                    "                  _ \n"
                    "                 | |\n"
                    "  _ __   ___  ___| |_ ___ _ __ ______ _ __  _ __\n"
                    " | '_ \\ / _ \\/ __| __/ _ \\ '__|______| '_ \\| '_ \\\n"
                    " | |_) | (_) \\__ \\ ||  __/ |         | | | | | | |\n"
                    " | .__/ \\___/|___/\\__\\___|_|         |_| |_|_| |_|\n"
                    " | |\n"
                    " |_|\n\n"
                );
                printf("\nThis command use a Self Organized Maps algorithm\n");
                printf("in order to create a posterization effect on an\n");
                printf("image given as input (with -i).\n");
                printf("The result is saved as a new image in\n");
                printf("<image_name>_posterized.<image_format>\n");
                printf("You can specify a custom output file name with -o.\n");
                printf("Supported formats are: jpeg, jpg, jpe, jp2, tiff,\n");
                printf("tif and png.\n");
                printf("But transparency is not correctly handle by default\n");
                printf("in OpenCV.\n\n");
                usage();
                exit(0);
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
                            "Expecting integer. Using default value.\n");
                }
                break;
            case 'e':
                tmp = (int)strtol(optarg, NULL, 10);
                if(tmp > 0){
                    *epochs = tmp;
                }
                else{
                    fprintf(stderr, "WARNING: Invalid argument for option -e. "\
                            "Expecting integer. Using default value.\n");
                }
                break;
            case 't':
                if(sscanf(optarg, "%f", &ftmp) != 0){
                    *thresh = ftmp;
                }
                else{
                    fprintf(stderr, "WARNING: Invalid argument for option -t. "\
                            "Expecting float. Using default value.\n");
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

/** The main function
 *
 * The main function scan the command line and set the program variables using
 * the specified options or the default values.
 * Then The function open the image from the input path and load it. Its pixels
 * are extracted.
 * The SOM network is trained from random values and using the image pixels.
 * The SOM resulting neurons represent the colors used for the posterization. So
 * the posterization function use the result of the SOM network to effectively
 * posterized the image. The image pixels are directly modified.
 * Finaly the modified image is saved as a new file.
 *
 * @note The number of neurons of the network is the posterization level power
 *  two. Which means that the SOM is square. It has a number of rows and a 
 *  number of columns equal to the posterization level. So it have
 *  rows * columns = rows^2 = columns^2 = posterization level^2 = number of 
 *  neurons.
 *
 * @warning The transparency in file formats which support it is not correctly
 *  handled.
 *
 * @todo 0.0.2
 *  Handle transparency with opencv and the SOM algorithm. The program 
 * currently use the number of channels of the image but it is always 3 (RGB) 
 * for now.
 */
int main(int argc, char * const argv[]){
    const char *ext;            // The file extension (image format)
    char saveName[PATH_MAX];    // Path to the saved posterized image
    unsigned int nbPixels;      // Number of pixels of the image
	float **origPixels;         // Pixels of the original, untouched image
    float **postPixels;         // Pixels of the posterized image
    float **trainRes;           // Output of the SOM (its map)
    int height;                 // Image height (in pixels)
    int width;                  // image width (in pixels)
    int channels;               // Number of channels of the image
    int i;                      // Iterator indice
    int j;                      // Iterator indice
    int nbNeurons;              // Number of neurons of the SOM
    int postLevel = 2;
    int epochs = 3000;
    float thresh = 0.001;
    char inFile[PATH_MAX] = {'\0'};
    char outFile[PATH_MAX] = {'\0'};
    IplImage *img;

    /* Set the program variables using the cmdl arguments */
    if(set_vars_from_args(argc, argv, &postLevel, &epochs, &thresh, inFile, 
                          outFile) > 0){
        return EXIT_FAILURE;
    }
    ext = get_filename_ext(inFile);
    nbNeurons = (int)pow(postLevel, 2);

    /* Load the image */
    img = cvLoadImage(inFile, CV_LOAD_IMAGE_COLOR);
    if(!img){
        printf("Image can not be loaded!\n");
        return 1;
    }

    /* Get the image data */
    height = img->height;
    width = img->width;
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
            uchar blue = CV_IMAGE_ELEM(img, uchar, j, i * 3 + 0);
            uchar green = CV_IMAGE_ELEM(img, uchar, j, i * 3 + 1);
            uchar red = CV_IMAGE_ELEM(img, uchar, j, i * 3 + 2);
            origPixels[i * height + j][0] = red / 255.;
            origPixels[i * height + j][1] = green / 255.;
            origPixels[i * height + j][2] = blue / 255.;
        }
    }
	postPixels = malloc(nbPixels * sizeof(float *));
	if(postPixels == NULL){
		fprintf(stderr, "out of memory\n");
		return EXIT_FAILURE;
    }
	for(i = 0; i < nbPixels; i++){
		postPixels[i] = malloc(channels * sizeof(float));
		if(postPixels[i] == NULL){
			fprintf(stderr, "out of memory\n");
			return EXIT_FAILURE;
        }
    }
    for(i = 0; i < width; i++){
        for(j = 0; j < height; j++){
            uchar blue = CV_IMAGE_ELEM(img, uchar, j, i * 3 + 0);
            uchar green = CV_IMAGE_ELEM(img, uchar, j, i * 3 + 1);
            uchar red = CV_IMAGE_ELEM(img, uchar, j, i * 3 + 2);
            postPixels[i * height + j][0] = red / 255.;
            postPixels[i * height + j][1] = green / 255.;
            postPixels[i * height + j][2] = blue / 255.;
        }
    }
    trainRes = malloc(channels * sizeof(float *));
    if(trainRes == NULL){
        fprintf(stderr, "out of memory\n");
        return EXIT_FAILURE;
    }
    for(i = 0; i < channels; i++){
        trainRes[i] = malloc(sizeof(float) * nbNeurons);
        if(trainRes[i] == NULL){
			fprintf(stderr, "out of memory\n");
			return EXIT_FAILURE;
        }
    }

    /* Train the network */
    som_train((float **)trainRes, origPixels, nbPixels, nbNeurons, epochs, 
              thresh);

    /* Posterize the image */
    som_posterize(postPixels, origPixels, trainRes, nbPixels, nbNeurons);
    arr_to_IplImage(img, postPixels, height, width, channels);

    /* Display the posterized image */
    cvNamedWindow("myfirstwindow", CV_WINDOW_AUTOSIZE);
    cvShowImage("myfirstwindow", img);
    cvWaitKey(0);

    /* Save the posterized image */
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
        free(postPixels[i]);
    }
    free(postPixels);
    for(i = 0; i < channels; i++){
        free(trainRes[i]);
    }
    free(trainRes);
    cvReleaseImage(&img);
    cvReleaseImageHeader(&img);
    cvDestroyWindow("myfirstwindow");

    return 0;
}
