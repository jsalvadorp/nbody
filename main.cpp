#include <iostream>
#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#define GLM_FORCE_RADIANS

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

using namespace std;
using namespace glm;

int star_count;
float *position;
float *velocity;
float *new_position;
float *new_velocity;

void draw() {
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glPointSize(1.0);
    glColor3f(1.0f, 1.0f, 1.0f);
    
    glBegin(GL_POINTS);
    for(int i = 0; i < star_count; i += 4) {
        glVertex3fv(position + i);
    }
    glEnd();

    glutSwapBuffers();
}

void allocateStars(int stars) {
    position = new float[4 * stars];
    velocity = new float[4 * stars];

    new_position = new float[4 * stars];
    new_velocity = new float[4 * stars];
    
    star_count = 0;
}

void placeDisk(int stars, vec3 pos, float radius, float min_mass, float max_mass) {
    int slices = 10, stars_per_slice = stars / slices;
    float slice_angle = 2 * M_PI / slices;
    
    for(int i = 0; i < slices; i++) {
        for(int j = 0; j < stars_per_slice; j++) {
            float r = (float(rand()) / RAND_MAX) * radius;
            float theta = (i * slice_angle) + (float(rand()) / RAND_MAX) * slice_angle;

            float x = pos[0] + r * cos(theta);
            float y = pos[1] + r * sin(theta);
            float z = pos[2] + (float(rand()) / RAND_MAX) * cos(2 * M_PI * r / radius) * radius;
            position[4 * star_count] = x;
            position[4 * star_count + 1] = y;
            position[4 * star_count + 2] = z;
            position[4 * star_count + 3] = min_mass + (float(rand()) / RAND_MAX) * (max_mass - min_mass);
            new_position[4 * star_count + 3] = position[4 * star_count + 3];

            cout << x << ", " << y << ", " << z << endl;

            star_count++;
            // velocity
        }
    }
}

void reshape (int w, int h) {
    glViewport (0, 0, (GLsizei) w, (GLsizei) h);
    glMatrixMode (GL_PROJECTION);
    glLoadIdentity ();

    gluPerspective(45, ((GLfloat)w) / h,  0.5, 100);
       
}

void applyCam() {
    glMatrixMode (GL_MODELVIEW);
    glLoadIdentity ();
    
    mat4 transform = lookAt(vec3(0.0f, 0.0f, 70.0f), vec3(0.0f,0.0f,0.0f), vec3(0.0f, 1.0f, 0.0f));

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    //|gluLookAt(0,0,1,0,0,0,0,1,0);
    glLoadMatrixf(&transform[0][0]);
    
}

void update() {
    glutPostRedisplay();
}

void timer(int value) {
    update(); 
}

void init(int stars) {
    allocateStars(stars);
    placeDisk(stars, vec3(0.0f, 0.0f, 0.0f), 60.0f, 10.0f, 1000.0f); 
}

int main(int argc, char **argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(600, 600); 
    glutCreateWindow(argv[0]);
    init(9000);
    glutDisplayFunc(draw); 
    glutTimerFunc(50, timer, 1);
    glutReshapeFunc(reshape);
    glutIgnoreKeyRepeat(true);
    
    glutMainLoop();
    //glutKeyboardFunc(keyDown);
    //glutKeyboardUpFunc(keyUp);
    //glutMouseFunc(mouse);
}
