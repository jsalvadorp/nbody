
unamestr=`uname -s`
if [[ "$unamestr" == 'Linux' ]]; then
    clear;g++ -g -DTOTAL_STARS=1200 -std=c++11 nbody.cpp main.cpp -lOpenCL -lGL -lGLU -lglut -op&&./p
elif [[ "$unamestr" == 'Darwin' ]]; then
    clear;g++ -g -DTOTAL_STARS=1200 -std=c++11 nbody.cpp main.cpp -framework OpenCL -framework OpenGL -framework GLUT -op&&./p
fi


