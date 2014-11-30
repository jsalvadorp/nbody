#pragma once

#include <cstdlib>
#include <iostream>
#include <iomanip>
#include <cstring>
#include <cassert>
#include <stdio.h>
#include <algrithm>
#include <sys/timeb.h>
#ifdef __APPLE__
#include <OpenCL/cl.h>
#else
#include <CL/cl.h>
#endif

using namespace std;

extern int star_count;
extern float *position;
extern float *velocity;
extern float *new_position;
extern float *new_velocity;


struct CLState { 
    cl_uint numPlatforms;
    cl_platform_id *platforms;
    cl_uint numDevices;
    cl_device_id *devices;
    cl_context context;
    cl_command_queue cmdQueue;

    cl_mem buf_position;
    cl_mem buf_velocity;
    cl_mem buf_new_position;
    cl_mem buf_new_velocity;

    cl_program program;
    cl_kernel kernel;
    
    int star_count;

    size_t global_work_size, local_work_size;

    cl_int initOpenCL() ;


    
    cl_int createBuffers(float *pos, float *vel, float *new_pos, float *new_vel, int stars) ;

    cl_int call(float time, float *pos, float *vel) ;

    void free() ;

}


