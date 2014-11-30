#include <cstdlib>
#include <iostream>
#include <iomanip>
#include <cstring>
#include <cassert>
#include <stdio.h>
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


struct cl_state { 
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

    size_t global_work_size, local_work_size;

    cl_int initOpenCL() {
        cl_int status;
        
        //PASO 1: Identificar e inicializar las plataformas
        numPlatforms = 0;
        *platforms = NULL;

        status = clGetPlatformIDs(0,NULL,&numPlatforms);
        platforms = (cl_platform_id*)malloc(numPlatforms*sizeof(cl_platform_id));
        status = clGetPlatformIDs(numPlatforms,platforms,NULL);

        //PASO 2: Identificar e inicializar los dispositivos
        numDevices = 0;
        *devices = NULL;

        //Usando el GPU
        status = clGetDeviceIDs(platforms[0],CL_DEVICE_TYPE_GPU,0,NULL,&numDevices);
        //Usando el CPU
    //	status=clGetDeviceIDs(platforms[1],CL_DEVICE_TYPE_CPU,0,NULL,&numDevices);

        devices = (cl_device_id*)malloc(numDevices*sizeof(cl_device_id));
        //Usando el GPU
        status = clGetDeviceIDs(platforms[0],CL_DEVICE_TYPE_GPU,numDevices,devices,NULL);
        //Usando el CPU
    //	status=clGetDeviceIDs(platforms[1],CL_DEVICE_TYPE_CPU,numDevices,devices,NULL);

        //PASO 3: Crear el contexto
        context = NULL;
        context = clCreateContext(NULL,numDevices,devices,NULL,NULL,&status);

        //PASO 4: Crear la fila de ordenes ("command queue")
        cmdQueue;
        cmdQueue = clCreateCommandQueue(context,devices[0],0,&status);

        //PASO 7: Crear y compilar el programa
        program = clCreateProgramWithSource(context,1,(const char**)&programSource,NULL,&status);
        status = clBuildProgram(program,numDevices,devices,NULL,NULL,NULL);

        if (status!=CL_SUCCESS) {
            return(0); 
        }

        //PASO 8: Crear el kernel
        kernel = NULL;
        kernel = clCreateKernel(program,"nbody",&status);


        return status;
    }


    
    cl_int createBuffers(float *pos, float *vel, float *new_pos, float *new_vel, int stars) {

        //Paso 10: Configurar la estructura del "work-item"
        size_t global_work_size[2]={};
        size_t local_work_size[2]={16,16};

        int size = 4 * stars * sizeof(float);
        //PASO 5: Crear los "device buffers"
        buf_position = clCreateBuffer(context,CL_MEM_READ_WRITE,size,NULL,&status);
        buf_velocity = clCreateBuffer(context,CL_MEM_READ_WRITE,size,NULL,&status);
        buf_new_position = clCreateBuffer(context,CL_MEM_READ_WRITE,size, NULL,&status);
        buf_new_velocity = clCreateBuffer(context,CL_MEM_READ_WRITE,size,NULL,&status);

        //PASO 6: Pasar los datos de la memoria del CPU a los "buffers" del dispositivo
        status = clEnqueueWriteBuffer(cmdQueue,buf_position,CL_FALSE,0,size,pos,0,NULL,NULL);
        status = clEnqueueWriteBuffer(cmdQueue,buf_new_position,CL_FALSE,0,size,vel,0,NULL,NULL);
        status = clEnqueueWriteBuffer(cmdQueue,buf_velocity,CL_FALSE,0,size,new_pos,0,NULL,NULL);
        status = clEnqueueWriteBuffer(cmdQueue,buf_new_velocity,CL_FALSE,0,size,new_vel,0,NULL,NULL);

        int n = N;
        //PASO 9: Pasar argumentos al kernel
        status = clSetKernelArg(kernel,0,sizeof(cl_mem),&bufferA);
        status |= clSetKernelArg(kernel,1,sizeof(cl_mem),&bufferB);
        status |= clSetKernelArg(kernel,2,sizeof(int),&n);
        status |= clSetKernelArg(kernel,3,sizeof(cl_mem),&bufferC);

        if (status != CL_SUCCESS) {
            return(0); 
           
        }

        //PASO 11: Enfilar el "kernel" para ejecucion
        status=clEnqueueNDRangeKernel(cmdQueue,kernel,2,NULL,globalWorkSize,localWorkSize,0,NULL,NULL);

        if (status!=CL_SUCCESS) {
            return(0); 
           
        }

        //PASO 12: Transportar el "buffer" de resultado a la memoria del CPU
        clEnqueueReadBuffer(cmdQueue,bufferC,CL_TRUE,0,N*N*sizeof(float),&V3_H,0,NULL,NULL);
        
        
        int i = 5, j = 10;
        std::cout << V3_H[i * N + j]<<endl;


        //PASO 13: Liberar recursos OpenCL
        clReleaseKernel(kernel);
        clReleaseProgram(program);
        clReleaseCommandQueue(cmdQueue);
        clReleaseMemObject(bufferA);
        clReleaseMemObject(bufferB);
        clReleaseMemObject(bufferC);
        clReleaseContext(context);

        free(platforms);
        free(devices);

    }

    void free() {
        
    }
