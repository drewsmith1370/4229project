#define GLFW 
#define USEGLEW 
#include "CSCIx229.h"
GLFWwindow* window = NULL;
// Keys
bool keys[256];
// View
int proj = 0;
double asp = 1;
int dim = 1;
int fov = 59;
// Camera
double camera[3] = { 0,0,-1 };

void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);
    glLoadIdentity();

    gluLookAt(0,0,-1 , 0,0,0 , 0,1,0);

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
    if (action == GLFW_RELEASE) {
        keys[key] = false;
    }

    //  Exit on ESC
   if (key == GLFW_KEY_ESCAPE)
      glfwSetWindowShouldClose(window,1);
//    //  Right arrow key - increase angle by 5 degrees
//    else if (key == GLFW_KEY_RIGHT)
   
//    //  Left arrow key - decrease angle by 5 degrees
//    else if (key == GLFW_KEY_LEFT)
   
//    //  Up arrow key - increase elevation by 5 degrees
//    else if (key == GLFW_KEY_UP)
//       ph += 5;
//    //  Down arrow key - decrease elevation by 5 degrees
//    else if (key == GLFW_KEY_DOWN)
//       ph -= 5;

    if (action == GLFW_PRESS && key < 256) {
        keys[key] = true;
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

    // Initialize GLEW
    // GLenum glewError = glewInit();
    // if (glewError != GLEW_OK) {
    //     printf("Failed to initialize GLEW: %s\n", glewGetErrorString(glewError));
    //     glfwTerminate();
    //     return -1;
    // }

    //  Set callbacks for window reshape and keyboard
    ClearKeys();
    glfwSetErrorCallback(errorCallback);
    glfwSetFramebufferSizeCallback(window,reshape);
    glfwSetKeyCallback(window,handleKey);

    //  Main loop
    while (!glfwWindowShouldClose(window))
    {
        display();
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    //  Quit
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}