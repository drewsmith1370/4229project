#include "CSCIx229.h"

// Window
int dim = 1;
int asp = 2;
float w = 1;
int fov = 59;
// Keys
bool keys[256] = {0};
// Time
double  progTime = 0;
double   aniTime = 0;
double deltaTime = 0;
bool playingTime = 0;
// Camera
double cam[3] = {0,0,2};
double th = 0;
double ph = 90;

/*
 *  Handle continuous key events
 */
void handleKeys() {
    for (int i=0;i<256;i++) {
        if (!keys[i]) continue;
        switch(i) {
            case 'w':
                cam[2] += 1;
                break;
            case 'a':
                cam[0] -= 1;
                printf("a");
                break;
            case 's':
                cam[2] -= 1;
                break;
            case 'd':
                cam[0] += 1;
                break;
            default:
                break;
        }
    }
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
    handleKeys();
    Project(fov,asp,dim);
    gluLookAt(cam[0],cam[1],cam[2] , 0,0,0 , 0,1,0);

    // Determine look at point with spherical coordinates, rx is phi ry is theta
    // double dx = cam[0] + Sin(th)*Cos(ph);
    // double dy = cam[1] - Sin(ph);
    // double dz = cam[2] - Cos(th)*Cos(ph);

    // gluLookAt(cam[0],cam[1],cam[2] , dx,dy,dz , 0,Cos(ph),0);
    
    glBegin(GL_POLYGON);
    glVertex3f( 0, 1, 0);
    glVertex3f(-1,-1, 0);
    glVertex3f( 1,-1, 0);
    glEnd();

    ErrCheck("display");
    glFlush();
    glutSwapBuffers();
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

/*
 *  Key event callback
 */
void keydown(unsigned char ch,int x,int y) {
   if (ch == 27) exit(0);
   keys[ch] = true;
//    if (ch == 'm')
// 	   mode = !mode;
//    if (ch == ' ')
//       moveTime = !moveTime;
//    if (ch == 'n')
//       showingField = !showingField;
}
void keyup(unsigned char ch,int x,int y) {
   keys[ch] = false;
}

/*
 *  GLUT calls this routine when the window is resized
 */
void reshape(int width,int height)
{
   //  Set the viewport to the entire window
   glViewport(0,0, RES*width,RES*height);
   //  Tell OpenGL we want to manipulate the projection matrix
   glMatrixMode(GL_PROJECTION);
   //  Undo previous transformations
   glLoadIdentity();
   //  Orthogonal projection box adjusted for the
   //  aspect ratio of the window
   asp = (height>0) ? (double)width/height : 1;
   //  Switch to manipulating the model matrix
   glMatrixMode(GL_MODELVIEW);
   Project(fov, asp, w);
   //  Undo previous transformations
   glLoadIdentity();
}

/*
 *  GLUT callback to update program time, and animation time if not paused
 */
void idle() {
    // Update time and deltaTime
    double now = glutGet(GLUT_ELAPSED_TIME)/1000.0;
    deltaTime = now - progTime;
    progTime = now;
    // Return if time is paused
    if (!playingTime) return;
    // Increment animation time
    aniTime += deltaTime;
    // Set screen to be redisplayed
    glutPostRedisplay();
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE);
    glutInitWindowSize(420,380);
    glutCreateWindow("Ethan Coleman, Drew Smith");

    glutDisplayFunc(display);
    glutIdleFunc(idle);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keydown);
    glutKeyboardUpFunc(keyup);

    // Set keys array
    for (int i=0;i<256;i++)
        keys[i] = false;  

    glEnable(GL_DEPTH_TEST);
    Project(fov,asp,dim);
    ErrCheck("init");
    glutMainLoop();
}