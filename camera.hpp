
struct Camera {
    //GLfloat x, y, z;
    //GLfloat pitch, yaw, roll;
    
    //GLfloat up[4], fwd[4], right[4];
    //GLfloat pos[4];

    vec3 pos, up, right, fwd;

    void aplicarSoloRotacion() { 
        mat4 transform = lookAt(vec3(0,0,0), fwd, up);

        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        //|gluLookAt(0,0,1,0,0,0,0,1,0);
        glLoadMatrixf(&transform[0][0]);

    }

    void aplicar() { // aplica las transformaciones de la camara
        mat4 transform = lookAt(pos, pos + fwd, up);

        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        //|gluLookAt(0,0,1,0,0,0,0,1,0);
        glLoadMatrixf(&transform[0][0]);
    }

    void mover(float r, float f, float u) {
        pos += up * u + fwd * f + right * r;
    }
    
    void yaw(float angle) { 
        fwd = apply(rotation(up, angle), fwd);
        right = normalize(cross(fwd, up));
    }

    void pitch(float angle) { 
        fwd = apply(rotation(right, angle), fwd);
        up = normalize(cross(right, fwd));
    }

    void roll(float angle) {
        up = apply(rotation(fwd, angle), up);
        right = normalize(cross(fwd, up));
    }

    void rotarZ(float angle) { // correra riesgo de imprecision?
        up = apply(rotation(vec3(0.0f, 0.0f, 1.0f), angle), up);
        fwd = apply(rotation(vec3(0.0f, 0.0f, 1.0f), angle), fwd);
        right = normalize(cross(fwd, up));
    }
    
};
