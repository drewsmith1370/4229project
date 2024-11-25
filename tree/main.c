#define GLFW 
#include "CSCIx229.h"
GLFWwindow* window = NULL;
// Keys
bool keys[256];
bool la = false;
bool ra = false;
bool ua = false;
bool da = false;
// View
int proj = 1;
double asp = 1;
int dim = 1;
int fov = 59;
// Light
double ambient=0;
double diffuse=100;
double specular=100;
// Camera
double cam[3] = { 0,0,-1 };
double th=90, ph=0;
// Time
double progTime = 0;
double deltaTime = 0;
bool paused = false;
// Buffers
unsigned int planeVbo;

// Take cross product (a x b) and store result in argument 3
void Cross(double a[3], double b[3], double result[3]) {
    result[0] =  a[1]*b[2] - a[2]*b[1];
    result[1] = -a[0]*b[2] + a[2]*b[0];
    result[2] =  a[0]*b[1] - a[1]*b[0];
}

// Calculate the direction of the camera. Returns unit vector that must be added to camera to calculate view point
void LookDirection(float ph, float th, double lookDir[3]) {
    lookDir[0] =  Sin(th)*Cos(ph);
    lookDir[1] = -Sin(ph);
    lookDir[2] = -Cos(th)*Cos(ph);
}

void Camera() {
    Project(fov,asp,dim);
    double dir[3];
    LookDirection(ph,th,dir);
    gluLookAt(cam[0],cam[1],cam[2] , cam[0]+dir[0],cam[1]+dir[1],cam[2]+dir[2] , 0,1,0);
}

void Plane() {
    glColor3f(0,.8,0);
    glBegin(GL_TRIANGLES);
    glVertex3f(-2,-1,-2);
    glVertex3f(-2,-1, 2);
    glVertex3f( 2,-1,-2);

    glVertex3f( 2,-1,-2);
    glVertex3f(-2,-1, 2);
    glVertex3f( 2,-1, 2);
    glEnd();
}

float branchNormals[] = {

};

void Branch() {
    // Trunk
    glColor3f(1,0,0);
    glBegin(GL_QUAD_STRIP);
    glNormal3f(-.167, 0,-.167); glVertex3f(  -.167,-1,  -.167);
    glNormal3f(-.033, 0, .233); glVertex3f(  -.033,-1,   .233);
    glNormal3f(-.167,.5,-.167); glVertex3f(-.167/2, 1,-.167/2);
    glNormal3f(-.033,.5, .233); glVertex3f(-.033/2, 1, .233/2);

    glNormal3f(-.033, 0, .233); glVertex3f(  -.033,-1,   .233);
    glNormal3f( .233, 0,-.033); glVertex3f(   .233,-1,  -.033);
    glNormal3f(-.033,.5, .233); glVertex3f(-.033/2, 1, .233/2);
    glNormal3f( .233,.5,-.033); glVertex3f( .233/2, 1,-.033/2);

    glNormal3f( .233, 0,-.033); glVertex3f(   .233,-1,  -.033);
    glNormal3f(-.167, 0,-.167); glVertex3f(  -.167,-1,  -.167);
    glNormal3f( .233,.5,-.033); glVertex3f( .233/2, 1,-.033/2);
    glNormal3f(-.167,.5,-.167); glVertex3f(-.167/2, 1,-.167/2);
    glEnd();

    // Top
    glBegin(GL_TRIANGLE_FAN);
    glNormal3f( 0, 1, 0); glVertex3f( 0,1.3, 0);
    glNormal3f(-.033, .5, .233); glVertex3f(-.033/2, 1, .233/2);
    glNormal3f( .233, .5,-.033); glVertex3f( .233/2, 1,-.033/2);
    glNormal3f(-.167, .5,-.167); glVertex3f(-.167/2, 1,-.167/2);
    glNormal3f(-.033, .5, .233); glVertex3f(-.033/2, 1, .233/2);
    glEnd();

    // Leaf
    glLightModeli(GL_LIGHT_MODEL_TWO_SIDE,1);
    glColor3f(0,.6,0);
    glBegin(GL_POLYGON);
    glNormal3f(0,0,-1);
    glVertex3f(  0,1.2,0);
    glVertex3f(-.1,1.3,0);
    glVertex3f(  0,1.6,0);
    glVertex3f( .1,1.3,0);
    glEnd();
    glLightModeli(GL_LIGHT_MODEL_TWO_SIDE,0);
}

void Tree() {
    Branch();

    ErrCheck("tree");
}

void Light() {
    //  Translate intensity to color vectors
    float Ambient[]   = {ambient*.01,ambient*.01,ambient*.01,1.0};
    float Diffuse[]   = {diffuse*.01,diffuse*.01,diffuse*.01,1.0};
    float Specular[]  = {specular*.01,specular*.01,specular*.01,1.0};
    //  Light position
    float Position[]  = {2*Cos(progTime*50),1,2*Sin(progTime*50),1}; //{distance*Cos(zh),ylight,distance*Sin(zh),1.0};

    // Draw point light
    glDisable(GL_LIGHTING);
    glColor3f(1,1,1);
    glPointSize(5);
    glBegin(GL_POINTS);
    glVertex4fv(Position);
    glEnd();

    //  OpenGL should normalize normal vectors
    glEnable(GL_NORMALIZE);
    //  Enable lighting
    glEnable(GL_LIGHTING);
    //  Location of viewer for specular calculations
    glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER,1);
    //  glColor sets ambient and diffuse color materials
    glColorMaterial(GL_FRONT_AND_BACK,GL_AMBIENT_AND_DIFFUSE);
    glEnable(GL_COLOR_MATERIAL);
    //  Enable light 0
    glEnable(GL_LIGHT0);
    //  Set ambient, diffuse, specular components and position of light 0
    glLightfv(GL_LIGHT0,GL_AMBIENT ,Ambient);
    glLightfv(GL_LIGHT0,GL_DIFFUSE ,Diffuse);
    glLightfv(GL_LIGHT0,GL_SPECULAR,Specular);
    glLightfv(GL_LIGHT0,GL_POSITION,Position);

    ErrCheck("lighting");
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);
    glLoadIdentity();

    Camera();
    Light();

    Plane();
    Tree();

    ErrCheck("display"); // Check for gl errors
    glFlush();
}

void ClearKeys() {
    for (int i=0;i<256;i++) {
        keys[i] = 0;
    }
}

void handleKey(GLFWwindow* window,int key,int scancode,int action,int mods) {
    bool setTo;
    if (action == GLFW_PRESS) {
        setTo = true;
    }
    else if (action == GLFW_RELEASE) {
        setTo = false;
    }
    else return;


    if (key < 256) {
        keys[key] = setTo;
        return;
    }

    //  Right arrow key - increase angle by 5 degrees
    else if (key == GLFW_KEY_RIGHT)
        ra = setTo;
    else if (key == GLFW_KEY_LEFT)
        la = setTo;
    else if (key == GLFW_KEY_UP)
        ua = setTo;
    else if (key == GLFW_KEY_DOWN)
        da = setTo;

    //  Exit on ESC
    if (key == GLFW_KEY_ESCAPE)
        glfwSetWindowShouldClose(window,1);
    
}

void handleUserInputs() {

    // Special
    if (la) 
        th -= 1;
    if (ra) 
        th += 1;
    if (ua) 
        ph -= 1;
    if (da) 
        ph += 1;

    double dir[3];
    double side[3];
    double up[3] = {0,1,0};
    double speed = .05;
    for (int i=0;i<256;i++) {
        if (!keys[i]) continue;
        switch(i) {
            case 'W':
                LookDirection(ph,th, dir);
                cam[0] += speed * dir[0];
                cam[1] += speed * dir[1];
                cam[2] += speed * dir[2];
                break;
            case 'S':
                LookDirection(ph,th, dir);
                cam[0] -= speed * dir[0];
                cam[1] -= speed * dir[1];
                cam[2] -= speed * dir[2];
                break;
            case 'A':
                LookDirection(ph,th,dir);
                Cross(up,dir,side);
                cam[0] += speed * side[0];
                cam[1] += speed * side[1];
                cam[2] += speed * side[2];
                break;
            case 'D':
                LookDirection(ph,th,dir);
                Cross(dir,up,side);
                cam[0] += speed * side[0];
                cam[1] += speed * side[1];
                cam[2] += speed * side[2];
                break;
            default: break;
        }
    }

}

//
//  Error callback for GLFW
//
static void errorCallback(int error, const char* description)
{
    fprintf(stderr, "Error: %s\n", description);
}

//
//  Callback for window resized
//
static void reshape(GLFWwindow* window,int width,int height)
{
    //  Ratio of the width to the height of the window
   asp = (height>0) ? (double)width/height : 1;
   //  Set the viewport to the entire window
   glViewport(0,0, RES*width,RES*height);
   //  Set projection
   Project(proj?fov:0,asp,dim);
}

void updateTime() {
    if (paused) return;
   // Update time and deltaTime
   double now = glfwGetTime();
   deltaTime = now - progTime;
   progTime = now;
}

int main(int argc, char** argv) {
    if (!glfwInit()) {
        fprintf(stderr,"Cannot Initialize GLFW");
        exit(1);
    }

    glfwWindowHint(GLFW_RESIZABLE,GLFW_TRUE);
    window = glfwCreateWindow(600,600,"Drew Smith, Ethan Coleman",NULL,NULL);
    if (window == NULL) {
        fprintf(stderr,"Cannot create window");
        glfwTerminate();
        exit(1);
    }

    // Set glfw context as current
    glfwMakeContextCurrent(window);

    glClearColor(.1,.4,1,1);

    //  Set callbacks for window reshape and keyboard
    ClearKeys();
    glfwSetErrorCallback(errorCallback);
    glfwSetFramebufferSizeCallback(window,reshape);
    glfwSetKeyCallback(window,handleKey);

    //  Main loop
    while (!glfwWindowShouldClose(window))
    {
        updateTime();
        handleUserInputs();
        display();
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    //  Quit
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}