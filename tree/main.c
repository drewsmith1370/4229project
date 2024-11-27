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
float lightPos[3] = {0};
// Camera
double cam[3] = { 0,0,-1 };
double th=90, ph=0;
// Time
double progTime = 0;
double deltaTime = 0;
bool paused = false;
// Buffers
unsigned int treeVao;
// Shaders
unsigned int shader;
unsigned int matrixUniform[2];
unsigned int lightUniform;

typedef struct Vertex_t {
    float p [3]; // pos
    float n [3]; // nrm
    float c [3]; // col
    float t [2]; // tex
} Vertex_t;

// VBO Data (Position, normal, color, and texture associated with each vertex)
Vertex_t treeBuffer[] = {
    // Base triangle
    {.p = {-.167,-1,-.167}, .n = {-.167, 0,-.167}, .c = {1,0,0}, .t = {0,0}}, // 0
    {.p = {-.033,-1, .233}, .n = {-.033, 0, .233}, .c = {1,0,0}, .t = {0,0}}, // 1
    {.p = { .233,-1,-.033}, .n = { .233, 0,-.033}, .c = {1,0,0}, .t = {0,0}}, // 2
    // Top triangle
    {.p = {-.167, 1,-.167}, .n = {-.167,.5,-.167}, .c = {1,0,0}, .t = {0,0}}, // 3
    {.p = {-.033, 1, .233}, .n = {-.033,.5, .233}, .c = {1,0,0}, .t = {0,0}}, // 4
    {.p = { .233, 1,-.033}, .n = { .233,.5,-.033}, .c = {1,0,0}, .t = {0,0}}, // 5
    // Tip
    {.p = {0,1.3,0},        .n = {0,1,0},          .c = {1,0,0}, .t = {0,0}}, // 6
};

// IBO Data (Order of vertices to draw tree as polygons)
int treeIdxBuffer[] = {
    0,1,2, // Bottom
    // Face 1
    0,3,1 , 3,4,1,
    // Face 2
    1,4,2 , 4,5,2,
    // Face 3
    2,5,0 , 5,3,0,
    // Tip
    3,6,4 , 4,6,5 , 5,6,3,
};

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
    glNormal3f(0,1,0);
    glBegin(GL_TRIANGLES);
    glVertex3f(-5,-1,-5);
    glVertex3f(-5,-1, 5);
    glVertex3f( 5,-1,-5);

    glVertex3f( 5,-1,-5);
    glVertex3f(-5,-1, 5);
    glVertex3f( 5,-1, 5);
    glEnd();
}

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
    glNormal3f( 0, 1, 0);        glVertex3f( 0,1.3, 0);
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
    // Fetch Modelview matrix (In future I might make my own matrix stack)
    GLfloat mvptr[16], projptr[16];
    glGetFloatv(GL_MODELVIEW_MATRIX,mvptr);
    glGetFloatv(GL_PROJECTION_MATRIX,projptr);

    // Use shader to draw buffer
    glUseProgram(shader);
    // Set Uniforms
    glUniformMatrix4fv(matrixUniform[0],1,false,mvptr);
    glUniformMatrix4fv(matrixUniform[1],1,false,projptr);
    glUniform3fv(lightUniform,1,lightPos);
    // Bind vao
    glBindVertexArray(treeVao);
    // Draw
    glDrawElements(GL_TRIANGLES, 30, GL_UNSIGNED_INT, 0);
    // Stop using shader
    glUseProgram(0);

    ErrCheck("tree");
    return;
}

void Light() {
    //  Translate intensity to color vectors
    float Ambient[]   = {ambient*.01,ambient*.01,ambient*.01,1.0};
    float Diffuse[]   = {diffuse*.01,diffuse*.01,diffuse*.01,1.0};
    float Specular[]  = {specular*.01,specular*.01,specular*.01,1.0};
    //  Light position
    float Position[]  = {2*Cos(progTime*50),1,2*Sin(progTime*50),1}; //{distance*Cos(zh),ylight,distance*Sin(zh),1.0};

    lightPos[0] = Position[0];
    lightPos[1] = Position[1];
    lightPos[2] = Position[2];

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
            case ' ':
                cam[1] += speed;
                break;
            case 'C':
                cam[1] -= speed;
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

/*
 *  Read text file
 */
char* ReadText(char *file)
{
   char* buffer;
   //  Open file
   FILE* f = fopen(file,"rt");
   if (!f) Fatal("Cannot open text file %s\n",file);
   //  Seek to end to determine size, then rewind
   fseek(f,0,SEEK_END);
   int n = ftell(f);
   rewind(f);
   //  Allocate memory for the whole file
   buffer = (char*)malloc(n+1);
   if (!buffer) Fatal("Cannot allocate %d bytes for text file %s\n",n+1,file);
   //  Snarf the file
   if (fread(buffer,n,1,f)!=1) Fatal("Cannot read %d bytes for text file %s\n",n,file);
   buffer[n] = 0;
   //  Close and return
   fclose(f);
   return buffer;
}

/*
 *  Print Shader Log
 */
void PrintShaderLog(int obj,char* file)
{
   int len=0;
   glGetShaderiv(obj,GL_INFO_LOG_LENGTH,&len);
   if (len>1)
   {
      int n=0;
      char* buffer = (char *)malloc(len);
      if (!buffer) Fatal("Cannot allocate %d bytes of text for shader log\n",len);
      glGetShaderInfoLog(obj,len,&n,buffer);
      fprintf(stderr,"%s:\n%s\n",file,buffer);
      free(buffer);
   }
   glGetShaderiv(obj,GL_COMPILE_STATUS,&len);
   if (!len) Fatal("Error compiling %s\n",file);
}

/*
 *  Print Program Log
 */
void PrintProgramLog(int obj)
{
   int len=0;
   glGetProgramiv(obj,GL_INFO_LOG_LENGTH,&len);
   if (len>1)
   {
      int n=0;
      char* buffer = (char *)malloc(len);
      if (!buffer) Fatal("Cannot allocate %d bytes of text for program log\n",len);
      glGetProgramInfoLog(obj,len,&n,buffer);
      fprintf(stderr,"%s\n",buffer);
   }
   glGetProgramiv(obj,GL_LINK_STATUS,&len);
   if (!len) Fatal("Error linking program\n");
}

/*
 *  Create Shader
 */
int CreateShader(GLenum type,char* file)
{
   //  Create the shader
   int shader = glCreateShader(type);
   //  Load source code from file
   char* source = ReadText(file);
   glShaderSource(shader,1,(const char**)&source,NULL);
   free(source);
   //  Compile the shader
   fprintf(stderr,"Compile %s\n",file);
   glCompileShader(shader);
   //  Check for errors
   PrintShaderLog(shader,file);
   //  Return name
   return shader;
}

/*
 *  Create Shader Program
 */
int CreateShaderProg(char* VertFile,char* FragFile)
{
   //  Create program
   int prog = glCreateProgram();
   //  Create and compile vertex shader
   int vert = CreateShader(GL_VERTEX_SHADER,VertFile);
   //  Create and compile fragment shader
   int frag = CreateShader(GL_FRAGMENT_SHADER,FragFile);
   //  Attach vertex shader
   glAttachShader(prog,vert);
   //  Attach fragment shader
   glAttachShader(prog,frag);
   //  Link program
   glLinkProgram(prog);
   //  Check for errors
   PrintProgramLog(prog);
   //  Return name
   return prog;
}

/*
 *  Create Geometry Shader Program
 */
int CreateGeomProg(char* VertFile,char* GeomFile,char* FragFile)
{
   //  Create program
   int prog = glCreateProgram();
   //  Create and compile vertex shader
   int vert = CreateShader(GL_VERTEX_SHADER  ,VertFile);
   //  Added Geometry shader
   int geom = CreateShader(GL_GEOMETRY_SHADER,GeomFile);
   //  Create and compile fragment shader
   int frag = CreateShader(GL_FRAGMENT_SHADER,FragFile);
   //  Attach vertex shader
   glAttachShader(prog,vert);
   //  Attach geometry shader
   glAttachShader(prog,geom);
   //  Attach fragment shader
   glAttachShader(prog,frag);
   //  Link program
   glLinkProgram(prog);
   //  Check for errors
   PrintProgramLog(prog);
   //  Return name
   return prog;
}


/*
 *  Create Compute Shader Program
 */
int CreateComputeProg(char* CompFile)
{
   int prog = glCreateProgram();
   int comp = CreateShader(GL_COMPUTE_SHADER, CompFile);

   glAttachShader(prog,comp);

   glLinkProgram(prog);
   PrintProgramLog(prog);
   
   return prog;
}

//
// Helper function to create a VAO with static draw for storing an object
//
GLuint CreateStaticVertexBuffer(int vsize, void* vdata, int isize, void* idata) {
    // Make vertex buffer object
    GLuint vbo;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, vsize, vdata, GL_STATIC_DRAW);

    // Make Index buffer object
    GLuint ibo;
    glGenBuffers(1, &ibo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, isize, idata, GL_STATIC_DRAW);

    // Combine as buffer array object
    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    // Bind VBO and IBO
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);

    return vao;
}

//
//  Compile and link shader programs, and initialize VAO for vertex data
//
void InitializeShadersAndVAO() {
    // VAO
    int vbosize = sizeof(treeBuffer);
    int ibosize = sizeof(treeIdxBuffer);
    treeVao = CreateStaticVertexBuffer(vbosize,treeBuffer , ibosize,treeIdxBuffer);
    

    // Make shader programs
    shader = CreateShaderProg("tree.vert","tree.frag");
    // Find uniforms
    matrixUniform[0] = glGetUniformLocation(shader,"ModelView");
    matrixUniform[1] = glGetUniformLocation(shader,"Projection");
    lightUniform = glGetUniformLocation(shader,"LightPos");

    // Get locations of attributes in shader
    int posLoc = glGetAttribLocation(shader,"position");
    int nrmLoc = glGetAttribLocation(shader,"normal");
    int colLoc = glGetAttribLocation(shader,"color");
    int texLoc = glGetAttribLocation(shader,"texture");
    printf("%i %i %i\n",nrmLoc,colLoc,texLoc);

    // Enable VAOs
    glEnableVertexAttribArray(posLoc);
    glEnableVertexAttribArray(nrmLoc);
    glEnableVertexAttribArray(colLoc);
    glEnableVertexAttribArray(texLoc);

    // Set vertex attribute pointers
    glVertexAttribPointer(posLoc, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex_t), (void*)offsetof(Vertex_t,p));
    glVertexAttribPointer(nrmLoc, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex_t), (void*)offsetof(Vertex_t,n));
    glVertexAttribPointer(colLoc, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex_t), (void*)offsetof(Vertex_t,c));
    glVertexAttribPointer(texLoc, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex_t), (void*)offsetof(Vertex_t,t));

    return;
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

    // Init shaders
    InitializeShadersAndVAO();

    ErrCheck("init");
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