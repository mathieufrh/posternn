cmake_minimum_required(VERSION 2.8)

project(posternn)

FILE(GLOB SRCS src/*.c)

add_executable(
    posternn
    ${SRCS})

add_definitions(-Wall -O3)

target_link_libraries(
        posternn
        GLU
        SDL
        SDL_mixer)

target_link_libraries(
    posternn
    opencv_core
    opencv_imgproc 
    opencv_highgui 
    opencv_ml 
    opencv_video 
    opencv_features2d 
    opencv_calib3d 
    opencv_objdetect 
    opencv_contrib 
    opencv_legacy 
    opencv_flann 
    m)
