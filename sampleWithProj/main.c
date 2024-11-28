#define GLFW 
#define USEGLEW 
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
// Camera
double cam[3] = { 0,0,-1 };
double th=90, ph=0;
// Time
double progTime = 0;
double deltaTime = 0;
bool paused = false;

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

void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);
    glLoadIdentity();

    Project(fov,asp,dim);
    double dir[3];
    LookDirection(ph,th,dir);
    gluLookAt(cam[0],cam[1],cam[2] , cam[0]+dir[0],cam[1]+dir[1],cam[2]+dir[2] , 0,1,0);

    glColor3f(1,1,1);
    glBegin(GL_POLYGON);
    glVertex2f(-1,-1);
    glVertex2f( 0, 1);
    glVertex2f( 1,-1);
    glEnd();

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