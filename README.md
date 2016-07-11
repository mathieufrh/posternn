[![Build Status](https://travis-ci.org/mathieufrh/posternn.svg?branch=master)](https://travis-ci.org/ntop/ntopng)
<a href="https://scan.coverity.com/projects/mathieufrh-posternn">
  <img alt="Coverity Scan Build Status"
       src="https://scan.coverity.com/projects/9395/badge.svg"/>
</a>
[![Code Climate](https://codeclimate.com/github/mathieufrh/posternn/badges/gpa.svg)](https://codeclimate.com/github/mathieufrh/posternn)
[![Test Coverage](https://codeclimate.com/github/mathieufrh/posternn/badges/coverage.svg)](https://codeclimate.com/github/mathieufrh/posternn/coverage)
[![Issue Count](https://codeclimate.com/github/mathieufrh/posternn/badges/issue_count.svg)](https://codeclimate.com/github/mathieufrh/posternn)

# DESCRIPTION

This program implement a self organized map (SOM) neural network in C and use
it to posterize images.
The SOM input is the image pixels RGB values. The network will iterate on the
input and process it until the defined number of iterations is reached. A new
image is then created using the network output which correspond to the
posterized image pixels RGB values.

For further details please see the report PDF file (french inside).

The program works pretty well with suported image formats without 
transparency.

# REQUIREMENT

To compile this program you'll need cmake. On debian you can use:
```
# apt-get install cmake
```

You'll also need OpenCV. On debian you can use:
```
# apt-get install libopencv-dev libcv-dev
```

# INSTALLATION

cd to the `posternn` directory then type the following command:
```
$ cmake . && make
```

Done!
The programm was successfully compiled and executed on:
 - debian 8
 - ubuntu 14.04
 - ubuntu 15.04
 - arch linux

If you encounter any issue during the installation please contact me at
mathieu.fourcroy@gmail.com

# USAGE EXAMPLE

The program expect one mandatory option:
 - -i which specify the image to posterized.
The two others options are optional:
 - -l specify the posterization level (the smaller the level the less colors in the posterized image). Default value is 2.
 - -e Specify the number of iterations of the network.
 - -t Specify the network threshold value. This is a stop condition for the iterating loop. If the network delta value ver fell under this threshold value, the loop is breaked.
 - -o Specify the output path of the posterized image. Default is the directory of the input image.

The 'imgs' folder contains a sample set of images. Each images comes with it posterized version. You can use one of these images to test the program or choose an image file on your machine. For instance:

```
$ ./posternn -i ./imgs/car.jpg
```

Which will output the file /imgs/car_posterized.jpg. The posterized image level is 2.

Or you can specify the posterization level and/or the output path:

```
$ ./posternn -i ./imgs/car.jpg -l 4 -o ./newdir/car_posterized_level_4.jpg
```
