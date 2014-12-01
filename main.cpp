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
float *position;
float *velocity;
float *new_position;
float *new_velocity;



void allocateStars(int stars) {
    position = new float[4 * stars];
    velocity = new float[4 * stars];

    new_position = new float[4 * stars];
    new_velocity = new float[4 * stars];
    
    star_count = 0;
}

void placeDisk(int stars, vec3 pos, float radius, float min_mass, float max_mass) {
    int slices = 10, stars_per_slice = stars / slices;
    float slice_angle = 8 * M_PI / slices;
    #if 1
    position[4 * star_count    ] = -10;
    position[4 * star_count + 1] = 0;
    position[4 * star_count + 2] = 0;
    position[4 * star_count + 3] = 1e17f;         
    velocity[4 * star_count    ] = 0.0f;
    velocity[4 * star_count + 1] = 0.0f;
    velocity[4 * star_count + 2] = 0.0f;
    velocity[4 * star_count + 3] = 0.0f;
            new_position[4 * star_count + 3] = position[4 * star_count + 3];
    star_count++;
    position[4 * star_count    ] = 10;
    position[4 * star_count + 1] = 0;
    position[4 * star_count + 2] = 0;
    position[4 * star_count + 3] = 1e17f;         
    velocity[4 * star_count    ] = 0.0f;
    velocity[4 * star_count + 1] = 0.0f;
    velocity[4 * star_count + 2] = 0.0f;
    velocity[4 * star_count + 3] = 0.0f;
            new_position[4 * star_count + 3] = position[4 * star_count + 3];
    star_count++;
    return; 

    #endif

    for(int i = 0; i < slices; i++) {
        cout << cos(i * slice_angle) << "," << sin(i * slice_angle) << endl;
        for(int j = 0; j < stars_per_slice; j++) {
            float r = (float(rand()) / RAND_MAX) * radius;
            float theta = (i * slice_angle) + (float(rand()) / RAND_MAX) * slice_angle;

            float x = pos[0] + r * cos(theta);
            float y = pos[1] + r * sin(theta);
            float z = pos[2] + (float(rand()) / RAND_MAX) * cos(2 * M_PI * r / radius) * radius;
            float mass = min_mass + (float(rand()) / RAND_MAX) * (radius/r) * (max_mass - min_mass);

            position[4 * star_count    ] = x;
            position[4 * star_count + 1] = y;
            position[4 * star_count + 2] = z;
            position[4 * star_count + 3] = mass;         
            velocity[4 * star_count    ] = 0.0f;
            velocity[4 * star_count + 1] = 0.0f;
            velocity[4 * star_count + 2] = 0.0f;
            velocity[4 * star_count + 3] = 0.0f;
            new_position[4 * star_count + 3] = position[4 * star_count + 3];

            cout << x << ", " << y << ", " << z << " = " << mass << endl;

            star_count++;
            // velocity
        }
    }
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

bool draw_vectors = true;

void draw() {
    applyCam();
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glPointSize(3.0);
    glColor3f(1.0f, 1.0f, 1.0f);
    
    glBegin(GL_POINTS);
    for(int i = 0; i < star_count; i++) {
        int offset = 4*i;
        glPointSize(std::min(position[offset+3] / 100.0f, 1.0f));
        glVertex3fv(position + offset);
    }
    glEnd();
    
    if(draw_vectors) {
        glBegin(GL_LINES);
        for(int i = 0; i < star_count; i ++) {
            int offset = 4*i;
            glVertex3fv(position + offset);
            glVertex3f(position[offset] + 4*velocity[offset], position[offset+1] + 4*velocity[offset+1], position[offset+2] + 4*velocity[offset+2]);
        }
        glEnd();
    }


    glutSwapBuffers();
}

CL_State opencl;
#define TIME_DELTA 0.12f
void update() {
    cout << "pos 0 x " << position[0] << "pos 0 y " << position[1]<< "pos 0 z " << position[2] << " mas 0 x "<< position[3] << " vel 0 x "  << velocity[0]<< endl;
    cout << "pos 1 x " << position[4] << "pos 1 y " << position[5]<< "pos 1 z " << position[6]<< " mas 0 x "<< position[7] << " vel 1 x " << velocity[4]<< endl;
    cout << "update " << endl;
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
    placeDisk(stars, vec3(0.0f, 0.0f, 0.0f), 50.0f, 2.0f, 400.0f); 
    opencl.createBuffers(position, velocity, new_position, new_velocity, star_count);
}

int main(int argc, char **argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(600, 600); 
    glutCreateWindow(argv[0]);
    init(300);
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
