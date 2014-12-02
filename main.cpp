#include <iostream>
#include <random>
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
float (*color)[4];
float (*position)[4];
float (*velocity)[4];
float (*new_position)[4];
float (*new_velocity)[4];

#define X 0
#define Y 1
#define Z 2
#define W 3

#ifndef TOTAL_STARS
#define TOTAL_STARS 1200
#endif

void allocateStars(int stars) {
    position = new float[stars][4];
    color = new float[stars][4];
    velocity = new float[stars][4];

    new_position = new float[stars][4];
    new_velocity = new float[stars][4];
    
    star_count = 0;
}

double densidad(double rnd) {

    return cos(M_PI / 2 * rnd);    
}

std::random_device rd;
std::mt19937 e2(rd());

double densidad_normal() {
    std::normal_distribution<> normal_dist(0.0, 0.5);
    return std::abs(normal_dist(e2));
}

const double SQRT2PI = std::sqrt(2*M_PI);
const double INVSQRT2PI = 1.0 / SQRT2PI;

double densidad_e(double x) {
    return exp(-8*pow(x,2.0));
}

// https://github.com/tylertreat/OpenGL-Playground/blob/master/ortho_with_camera/Camera.h
mat4 rotation(const vec3& _axis, double theta) {
    vec3 axis = normalize(_axis);
    mat4 ret; // identity

    double radians    = glm::radians(theta);
    double cosine     = cos(radians);
    double cosine1    = 1 - cosine;
    double sine       = sin(radians);

    ret[0][0] = axis[0] * axis[0] + (1 - axis[0] * axis[0]) * cosine;
    ret[0][1] = cosine1 * axis[0] * axis[1] - axis[2] * sine;
    ret[0][2] = cosine1 * axis[0] * axis[2] + axis[1] * sine;
    ret[1][0] = cosine1 * axis[0] * axis[1] + axis[2] * sine;
    ret[1][1] = axis[1] * axis[1] + (1 - axis[1] * axis[1]) * cosine;
    ret[1][2] = cosine1 * axis[1] * axis[2] - axis[0] * sine;
    ret[2][0] = cosine1 * axis[0] * axis[2] - axis[1] * sine;
    ret[2][1] = cosine1 * axis[1] * axis[2] + axis[0] * sine;
    ret[2][2] = axis[2] * axis[2] + (1 - axis[2] * axis[2]) * cosine;

    return ret;
}

vec3 apply(const mat4& matrix, const vec3& v) {
    vec4 temp = matrix * vec4(v, 0.0);
    return vec3(temp.x, temp.y, temp.z);
}

void placeDisk(int stars, vec3 origen, float radius, float min_mass, float max_mass, float tan_vel, vec3 vel, float agujero, const mat4 &rot) {
    #if 0
        
    //return; 

    #endif

    for(int i = 0; i < stars; i++) {
        float r_rnd = densidad_normal();//(float(rand()) / RAND_MAX);
        float r = r_rnd * radius;
        float theta = 2 * M_PI * (float(rand()) / RAND_MAX);
        float mass = min_mass + (radius / r) * (float(rand()) / RAND_MAX) * (max_mass - min_mass);
        float x = r * cos(theta);
        float y = 0.6 * r * sin(theta);
        float z = densidad_e(float(rand()) / RAND_MAX) * r_rnd * radius/2 - radius / 4;

        cout << "x " <<x << " y " << y << " z " << z << endl;
        
        double e_color = 1.0f - r_rnd;
        color[star_count][0] = e_color;
        color[star_count][1] = e_color;
        color[star_count][2] = 1.0f; // blue
        color[star_count][3] = 1.0f; 


        if(i == 0) { // el agujero negro supermasivo en el centro
        
            position[star_count][X] = origen[0];
            position[star_count][Y] = origen[1];
            position[star_count][Z] = origen[2];
            position[star_count][W] = agujero * mass;//1e19f;   
        
            velocity[star_count][X] = vel[0] + -(r/radius)*sin(theta)*tan_vel;// * tan_vel - r * cos(theta) * ((float(rand()) / RAND_MAX) * tan_vel/8);//((float(rand()) / RAND_MAX) * tan_vel - tan_vel / 2);
            velocity[star_count][Y] = vel[1] + (r/radius)*cos(theta)*tan_vel;// * tan_vel  - r * sin(theta) * ((float(rand()) / RAND_MAX) * tan_vel/8);//((float(rand()) / RAND_MAX) * tan_vel - tan_vel / 2);
            velocity[star_count][Z] = vel[2] + ((float(rand()) / RAND_MAX) * tan_vel - tan_vel / 2);
            velocity[star_count][W] = 0.0f; 
        } else  {
            
            vec3 pos = apply(rot, vec3(x,y,z));
            
            position[star_count][X] = origen[0] + pos[0];
            position[star_count][Y] = origen[1] + pos[1];
            position[star_count][Z] = origen[2] + pos[2];
            position[star_count][W] = mass;//1e19f;   

            vec3 vel_i;
            
            vel_i[0] = -r_rnd*sin(theta)*tan_vel;
            vel_i[1] = r_rnd*cos(theta)*tan_vel;
            vel_i[2] = 0.0f;

            vel_i = apply(rot, vel_i);
        
            velocity[star_count][X] = vel[0] + vel_i[0];// * tan_vel - r * cos(theta) * ((float(rand()) / RAND_MAX) * tan_vel/8);//((float(rand()) / RAND_MAX) * tan_vel - tan_vel / 2);
            velocity[star_count][Y] = vel[1] + vel_i[1];// * tan_vel  - r * sin(theta) * ((float(rand()) / RAND_MAX) * tan_vel/8);//((float(rand()) / RAND_MAX) * tan_vel - tan_vel / 2);
            velocity[star_count][Z] = vel[2] + vel_i[2]; //((float(rand()) / RAND_MAX) * tan_vel - tan_vel / 2);
            velocity[star_count][W] = 0.0f; 
        }

        star_count++;
    }
    
    return;

 
}

void reshape (int w, int h) {
    glViewport (0, 0, (GLsizei) w, (GLsizei) h);
    glMatrixMode (GL_PROJECTION);
    glLoadIdentity ();

    gluPerspective(45, ((GLfloat)w) / h,  0.5, 500);
       
}

const vec3 center(0.0f);

vec3 eye(0.0f, 0.0f, 200.0f);
vec3 up(0.0f, 1.0f, 0.0f);
//vec3 right(1.0f, 0.0f, 0.0f);

void applyCam() {
    //glMatrixMode (GL_MODELVIEW);
    //glLoadIdentity ();
    
    mat4 transform = lookAt(eye, center, up);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    //|gluLookAt(0,0,1,0,0,0,0,1,0);
    glLoadMatrixf(&transform[0][0]);
    
}

bool draw_vectors = false;

float scalef = 1.0f;

void draw() {
    applyCam();
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glScalef(scalef, scalef, scalef);
    glPointSize(1.0f);
    
    glBegin(GL_POINTS);
    for(int i = 0; i < star_count; i++) {
        /*position[i][X] = std::min(55.0f, std::max(-55.0f, position[i][X]));
        position[i][Y] = std::min(55.0f, std::max(-55.0f, position[i][Y]));
        position[i][Z] = std::min(55.0f, std::max(-55.0f, position[i][Z]));
        velocity[i][X] = std::min(5.0f, std::max(-5.0f, position[i][X]));
        velocity[i][Y] = std::min(5.0f, std::max(-5.0f, position[i][Y]));
        velocity[i][Z] = std::min(5.0f, std::max(-5.0f, position[i][Z]));*/
        //cout << i << ": x " << position[i][X]  << ": y " << position[i][Y] << ": z " << position[i][Z] << ": mass " << position[i][W] << endl;
        //cout << i << ": velocity  x " << velocity[i][X]  << ": y " << velocity[i][Y] << ": z " << velocity[i][Z] << ": mass " << velocity[i][W] << endl;
        //glPointSize(1.0f);
        glColor3fv(color[i]);
        glVertex3fv(position[i]);
    }
    glEnd();


    glutSwapBuffers();
}

bool keys[256];

void keyDown(unsigned char key, int mouseX, int mouseY) {
    keys[tolower(key)] = keys[toupper(key)] = true;
    //teclas_opuestas[key][0] = teclas_opuestas[key][1] = false;
}

void keyUp(unsigned char key, int mouseX, int mouseY) {
    keys[tolower(key)] = keys[toupper(key)] = false;
}


        //fwd = apply(rotation(up, angle), fwd);
        //right = normalize(cross(fwd, up));

void checarTeclado() {
    if(keys['a']) {
        //vec3 fwd = normalize(center - eye);
        //vec3 right = normalize(cross(fwd, up));
        mat4 rot = rotation(up, -3);
        eye = apply(rot, eye);
        //right = normalize(apply(rot, right));

    }
    if(keys['d']) {
        //vec3 fwd = normalize(center - eye);
        //vec3 right = normalize(cross(fwd, up));
        mat4 rot = rotation(up, 3);
        eye = apply(rot, eye);
        //right = normalize(apply(rot, right));

    }
    if(keys['w']) {
        vec3 fwd = normalize(center - eye);
        vec3 right = normalize(cross(fwd, up));
        mat4 rot = rotation(right, -3);
        eye = apply(rot, eye);
        fwd = normalize(center - eye);
        up = normalize(cross(right, fwd));

        //right = normalize(apply(rot, right));

    }
    if(keys['s']) {
        vec3 fwd = normalize(center - eye);
        vec3 right = normalize(cross(fwd, up));
        mat4 rot = rotation(right, 3);
        eye = apply(rot, eye);
        fwd = normalize(center - eye);
        up = normalize(cross(right, fwd));

        //right = normalize(apply(rot, right));

    }
    if(keys['o']) {
        scalef *= 0.8;
    }
    if(keys['p']) {
        scalef *= 1.25;
    }
    
    if(keys[27]) {
        exit(0);
    }
         
}


CL_State opencl;
float time_delta = 1e-10f;
void update() {
    int offs = 0;
    //cout << "update " << endl;
    

    assert(std::abs(position[offs][X]) < 1000);
    opencl.call(time_delta, position, velocity);
    checarTeclado();
    
    glutPostRedisplay();
}

void timer(int value) {
    update(); 
    glutTimerFunc(50, timer, 1);
}






void init(int stars) {
    opencl.initOpenCL();


    allocateStars(stars);
    
    //void placeDisk(int stars, vec3 origen, float radius, float min_mass, float max_mass, float tan_vel, vec3 vel, float agujero, const mat4 &rot) {

    mat4 rot1 = rotation(vec3(1.0f, 1.0f, 0.0f), 35);
    placeDisk(stars/2, vec3(-40.0f, -40.0f, 0.0f), 12.0f, 1e21f, 1e27f, 600e7f, vec3(-40e7f, 40.2e7f, 0.5e7f), 7000.0f, rot1); 
    
    
    mat4 rot2 = rotation(vec3(-1.0f, 1.0f, 0.0f), 20);
    placeDisk(stars/2, vec3(40.0f, 40.0f, 0.0f), 12.0f, 1e21f, 1e27f, -333e7f, vec3(40e7f, -40.2e7f, -0.4e7f), 1500.0f, rot2); 
    
    
    opencl.createBuffers(position, velocity, new_position, new_velocity, star_count);
}

int main(int argc, char **argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(600, 600); 
    glutCreateWindow(argv[0]);
    init(TOTAL_STARS);
    glutDisplayFunc(draw); 
    glutTimerFunc(50, timer, 1);
    glutReshapeFunc(reshape);
    glutIgnoreKeyRepeat(true);
    glutKeyboardFunc(keyDown);
    glutKeyboardUpFunc(keyUp);
    
    glutMainLoop();

    opencl.free();
    //glutMouseFunc(mouse);
}
