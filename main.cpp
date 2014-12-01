#include <iostream>
#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#define _USE_MATH_DEFINES
#include <math.h>

#define GLM_FORCE_RADIANS

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "nbody.hpp"

using namespace std;
using namespace glm;

int star_count;
float (*position)[4];
float (*velocity)[4];
float (*new_position)[4];
float (*new_velocity)[4];

#define X 0
#define Y 1
#define Z 2
#define W 3


void allocateStars(int stars) {
    position = new float[stars][4];
    velocity = new float[stars][4];

    new_position = new float[stars][4];
    new_velocity = new float[stars][4];
    
    star_count = 0;
}

void placeDisk(int stars, vec3 pos, float radius, float min_mass, float max_mass /*, float radial_vel, vec3 vel*/) {
    #if 0
    position[star_count][X] = 0.0f;
    position[star_count][Y] = 0.0f;
    position[star_count][Z] = 0.0f;
    position[star_count][W] = 1e19f;
    
    velocity[star_count][X] = 0.0f;
    velocity[star_count][Y] = 1.0f;
    velocity[star_count][Z] = 2.0f;
    velocity[star_count][W] = 0.0f;

    star_count++;

    position[star_count][X] = (float(rand()) / RAND_MAX) * 5.0f;
    position[star_count][Y] = (float(rand()) / RAND_MAX) * 5.0f;
    position[star_count][Z] = (float(rand()) / RAND_MAX) * 5.0f;
    position[star_count][W] = 1e19f;
    
    velocity[star_count][X] = 0.0f;
    velocity[star_count][Y] = -1.0f;
    velocity[star_count][Z] = 2.0f;
    velocity[star_count][W] = 0.0f;

    star_count++;

    
    //return; 

    #endif

    for(int i = 0; i < stars; i++) {
        float r = (float(rand()) / RAND_MAX) * radius;
        float theta = 2 * M_PI * (float(rand()) / RAND_MAX);
        float mass = min_mass + (float(rand()) / RAND_MAX) * (max_mass - min_mass);
        float x = pos[0] + r * cos(theta);
        float y = pos[1] + r * sin(theta);
        float z = pos[2] + (float(rand()) / RAND_MAX) * cos(2 * M_PI * r / radius) * radius;

        cout << "x " <<x << " y " << y << " z " << z << endl;
        position[star_count][X] = x;
        position[star_count][Y] = y;
        position[star_count][Z] = z;
        position[star_count][W] = 1e19f;
        
        velocity[star_count][X] = 0.0f;
        velocity[star_count][Y] = 0.0f;
        velocity[star_count][Z] = 0.0f;
        velocity[star_count][W] = 0.0f;

        star_count++;
    }
    
    return;

 
}

void reshape (int w, int h) {
    glViewport (0, 0, (GLsizei) w, (GLsizei) h);
    glMatrixMode (GL_PROJECTION);
    glLoadIdentity ();

    gluPerspective(45, ((GLfloat)w) / h,  0.5, 300);
       
}

void applyCam() {
    //glMatrixMode (GL_MODELVIEW);
    //glLoadIdentity ();
    
    mat4 transform = lookAt(vec3(0.0f, 0.0f, 200.0f), vec3(0.0f,0.0f,0.0f), vec3(0.0f, 1.0f, 0.0f));

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    //|gluLookAt(0,0,1,0,0,0,0,1,0);
    glLoadMatrixf(&transform[0][0]);
    
}

bool draw_vectors = false;

void draw() {
    applyCam();
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glPointSize(3.0);
    glColor3f(1.0f, 1.0f, 1.0f);
    
    glBegin(GL_POINTS);
    for(int i = 0; i < star_count; i++) {
        /*position[i][X] = std::min(55.0f, std::max(-55.0f, position[i][X]));
        position[i][Y] = std::min(55.0f, std::max(-55.0f, position[i][Y]));
        position[i][Z] = std::min(55.0f, std::max(-55.0f, position[i][Z]));
        velocity[i][X] = std::min(5.0f, std::max(-5.0f, position[i][X]));
        velocity[i][Y] = std::min(5.0f, std::max(-5.0f, position[i][Y]));
        velocity[i][Z] = std::min(5.0f, std::max(-5.0f, position[i][Z]));*/
        cout << i << ": x " << position[i][X]  << ": y " << position[i][Y] << ": z " << position[i][Z] << ": mass " << position[i][W] << endl;
        glPointSize(std::min(position[i][W] / 100.0f, 1.0f));
        glVertex3fv(position[i]);
    }
    glEnd();
    
    if(draw_vectors) {
        glBegin(GL_LINES);
        for(int i = 0; i < star_count; i ++) {
            glVertex3fv(position[i]);
            //glVertex3f(position[offset] + 4*velocity[offset], position[offset+1] + 4*velocity[offset+1], position[offset+2] + 4*velocity[offset+2]);
        }
        glEnd();
    }


    glutSwapBuffers();
}

CL_State opencl;
#define TIME_DELTA 0.006f
void update() {
    int offs = 0;
    cout << "update " << endl;
    

    assert(std::abs(position[offs][X]) < 1000);
    opencl.call(TIME_DELTA, position, velocity);
    
    
    glutPostRedisplay();
}

void timer(int value) {
    update(); 
    glutTimerFunc(50, timer, 1);
}


void init(int stars) {
    opencl.initOpenCL();


    allocateStars(stars);
    placeDisk(stars/2, vec3(-40.0f, -40.0f, 0.0f), 15.0f, 1e20f, 9e20f); 
    placeDisk(stars/2, vec3(40.0f, 40.0f, 0.0f), 15.0f, 1e20f, 9e20f); 
    opencl.createBuffers(position, velocity, new_position, new_velocity, star_count);
}

int main(int argc, char **argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(600, 600); 
    glutCreateWindow(argv[0]);
    init(100);
    glutDisplayFunc(draw); 
    glutTimerFunc(50, timer, 1);
    glutReshapeFunc(reshape);
    glutIgnoreKeyRepeat(true);
    
    glutMainLoop();

    opencl.free();
    //glutKeyboardFunc(keyDown);
    //glutKeyboardUpFunc(keyUp);
    //glutMouseFunc(mouse);
}
