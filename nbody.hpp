#pragma once

#include <cstdlib>
#include <iostream>
#include <iomanip>
#include <cstring>
#include <cassert>
#include <stdio.h>
#include <stdlib.h>

#include <algorithm>
#include <sys/timeb.h>
#ifdef __APPLE__
#include <OpenCL/cl.h>
#else
#include <CL/cl.h>
#endif

using namespace std;


extern char *programSource;

class CL_State { 
public:
	cl_int status;

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

	size_t global_work_size[2];
	size_t local_work_size[2];

    cl_int initOpenCL() ;


    
    cl_int createBuffers(float (*pos)[4], float (*vel)[4], float (*new_pos)[4], float (*new_vel)[4], int stars);

    cl_int call(float time, float (*pos)[4], float (*vel)[4]);

    void free() ;

};


