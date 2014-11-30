#include "nbody.hpp"


cl_int CL_State::initOpenCL() {
	
	FILE *fp;
	char fileName[] = "nbody.ocl";
	size_t source_size; 
	/* Load the source code containing the kernel*/
	fp = fopen(fileName, "rb");
	if (!fp) {
		fprintf(stderr, "Failed to load kernel.\n");
		exit(1);
	}
	fseek(fp, 0, SEEK_END);
	long codeSize = ftell(fp);
	programSource = (char*) malloc( codeSize );
	source_size = fread(programSource, 1, codeSize, fp);
	fclose(fp);




    
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
//    status = clGetDeviceIDs(platforms[0],CL_DEVICE_TYPE_GPU,0,NULL,&numDevices);
    //Usando el CPU
	status=clGetDeviceIDs(platforms[1],CL_DEVICE_TYPE_CPU,0,NULL,&numDevices);

    devices = (cl_device_id*)malloc(numDevices*sizeof(cl_device_id));
    //Usando el GPU
//    status = clGetDeviceIDs(platforms[0],CL_DEVICE_TYPE_GPU,numDevices,devices,NULL);
    //Usando el CPU
	status=clGetDeviceIDs(platforms[1],CL_DEVICE_TYPE_CPU,numDevices,devices,NULL);

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
		printf("Failure in Create and Compile: %i", status);
        return(0); 
    }

    //PASO 8: Crear el kernel
    kernel = NULL;
    kernel = clCreateKernel(program,"nbody",&status);


    return status;
}



cl_int CL_State::createBuffers(float *pos, float *vel, float *new_pos, float *new_vel, int stars) {
    star_count = stars;
    //Paso 10: Configurar la estructura del "work-item"
    global_work_size[0] = stars; global_work_size[1] = 1;
    local_work_size[0] = 1; local_work_size[1] = 1;

    int size = 4 * stars * sizeof(float); //why multiplicated with 4????
    //PASO 5: Crear los "device buffers"
    buf_position = clCreateBuffer(context,CL_MEM_READ_WRITE,size,NULL,&status);
    buf_velocity = clCreateBuffer(context,CL_MEM_READ_WRITE,size,NULL,&status);
    buf_new_position = clCreateBuffer(context,CL_MEM_READ_WRITE,size, NULL,&status);
    buf_new_velocity = clCreateBuffer(context,CL_MEM_READ_WRITE,size,NULL,&status);

    //PASO 6: Pasar los datos de la memoria del CPU a los "buffers" del dispositivo
    status = clEnqueueWriteBuffer(cmdQueue, buf_position, CL_FALSE, 0, size, pos, 0, NULL, NULL);
    status = clEnqueueWriteBuffer(cmdQueue, buf_new_position, CL_FALSE, 0,size, vel, 0, NULL, NULL);
    status = clEnqueueWriteBuffer(cmdQueue, buf_velocity, CL_FALSE, 0, size, new_pos, 0, NULL, NULL);
    status = clEnqueueWriteBuffer(cmdQueue, buf_new_velocity, CL_FALSE, 0, size, new_vel, 0, NULL, NULL);
    return status;
}

cl_int CL_State::call(float time, float *pos, float *vel) {
    int size = star_count * 4 * sizeof(float);
    //int n = N;
    //PASO 9: Pasar argumentos al kernel
    status = clSetKernelArg(kernel,0,sizeof(cl_mem),&buf_position);
    status |= clSetKernelArg(kernel,1,sizeof(cl_mem),&buf_velocity);
    status |= clSetKernelArg(kernel,2,sizeof(cl_mem),&buf_new_position);
    status |= clSetKernelArg(kernel,3,sizeof(cl_mem),&buf_new_velocity);
    status |= clSetKernelArg(kernel,4,sizeof(int),&star_count);
    status |= clSetKernelArg(kernel,5,sizeof(float),&time);

    if (status != CL_SUCCESS) {
        return status; 
       
    }

    //PASO 11: Enfilar el "kernel" para ejecucion
    status=clEnqueueNDRangeKernel(cmdQueue,kernel,2,NULL,global_work_size,local_work_size,0,NULL,NULL);

    if (status!=CL_SUCCESS) {
        return(0); 
       
    }

    //PASO 12: Transportar el "buffer" de resultado a la memoria del CPU
    clEnqueueReadBuffer(cmdQueue,buf_new_position,CL_TRUE,0,size,pos,0,NULL,NULL);
    clEnqueueReadBuffer(cmdQueue,buf_new_velocity,CL_TRUE,0,size,vel,0,NULL,NULL);
    
    
    // importante: swap new and old
    swap(buf_new_position, buf_position);
    swap(buf_new_velocity, buf_velocity);
    
	return 0;
}

void CL_State::free() {
    
    //PASO 13: Liberar recursos OpenCL
    clReleaseKernel(kernel);
    clReleaseProgram(program);
    clReleaseCommandQueue(cmdQueue);

    clReleaseMemObject(buf_position);
	clReleaseMemObject(buf_velocity);
	clReleaseMemObject(buf_new_position);
	clReleaseMemObject(buf_new_velocity);

    clReleaseContext(context);

    //free(platforms);
    //free(devices);
}

