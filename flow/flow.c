#include "CSCIx229.h"
#define PARTICLE_MODE 0
#define DOT_MODE 1
#define NUM_DOTS 32768

/*  
 *  Buffer layout (all floats):
 *  dotPositions  (vec4) | velocities  (vec2) | prevPositions (vec4)
 *  [x y z w] * NUM_DOTS | [ux uy] * NUM_DOTS | [x y z w] * NUM_DOTS
 */
typedef struct DotBuffer_t {
   float dotPositions  [4 * NUM_DOTS];
   float velocities    [2 * NUM_DOTS];
   float prevPositions [4 * NUM_DOTS];
} DotBuffer_t;

/*
 *  State Variables
 */

// Shaders
int shader[]  = {0,0,0,0}; // Shaders
unsigned int noise;
unsigned int timeUniformLocation[2];
unsigned int deltaTimeUniform;
// Time
bool moveTime = true;
double progTime = 0;
double deltaTime = 0;
// Window
double asp=1;
double dim=1;
bool keys[256];
// Controls
int mode = PARTICLE_MODE;
bool showingField = false;

/*
 *  Display Function
 */
void display() {
   glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
   glEnable(GL_DEPTH_TEST);
   glPointSize(3);
   glLoadIdentity();
   glLineWidth(2);

   // Compute shader to determine dot positions
   glUseProgram(shader[0]);
   glUniform1f(timeUniformLocation[0], (float)progTime);
   glUniform1f(deltaTimeUniform, deltaTime);
   glDispatchCompute(32,1,1);
   glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

   if (mode == PARTICLE_MODE)
   {
      // Render program to display dots
      glUseProgram(shader[3]);
      glDrawArrays(GL_POINTS,0,NUM_DOTS);
   }
   else {
      glUseProgram(shader[1]);
      glDrawArrays(GL_POINTS,0,NUM_DOTS);
   }
   if (showingField) {
      // Draw noise program on a quad
      glUseProgram(shader[2]);
      glUniform1f(timeUniformLocation[1], (float)progTime);
      glBegin(GL_TRIANGLES);
      // left triangle
      glVertex2f(-1, 1);
      glVertex2f( 1,-1);
      glVertex2f(-1,-1);
      // right triangle
      glVertex2f(-1, 1);
      glVertex2f( 1, 1);
      glVertex2f( 1,-1);
      glEnd();
   }

   glUseProgram(0);
   glColor3f(1,1,1);
   glWindowPos2i(6,6);
   Print("Time is: %.1f", progTime);
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
   
   if (ch == 'm')
	   mode = !mode;
   if (ch == ' ')
      moveTime = !moveTime;
   if (ch == 'n')
      showingField = !showingField;
}
void keyup(unsigned char ch,int x,int y) {
   keys[ch] = false;
}

/*
 *  GLUT calls this routine when the window is resized
 */
void reshape(int width,int height)
{
   //  Ratio of the width to the height of the window
   asp = (height>0) ? (double)width/height : 1;
   //  Set the viewport to the entire window
   glViewport(0,0, RES*width,RES*height);
   //  Set projection
   Project(0,asp,dim);
}

/*
 *  GLUT callback to update time
 */
void idle() {
   if (!moveTime) return;
   // Update time and deltaTime
   double now = glutGet(GLUT_ELAPSED_TIME)/1000.0;
   deltaTime = now - progTime;
   progTime = now;
   // Set screen to be redisplayed
   glutPostRedisplay();
}

/*
 *  Main function - initialize shaders and glut
 */
int main(int argc, char** argv) {
   srand(time(NULL));
   // Initialize keys to false
   for (int i=0;i<256;i++) keys[i] = 0;

   glutInit(&argc,argv);
   glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE);
   glutInitWindowSize(420,380);
   glutCreateWindow("Ethan Coleman, Drew Smith");

#ifdef USEGLEW
   //  Initialize GLEW
   if (glewInit()!=GLEW_OK) Fatal("Error initializing GLEW\n");
#endif

   //  Set callbacks
   glutDisplayFunc(display);
   glutReshapeFunc(reshape);
   // glutSpecialFunc(special);
   glutKeyboardFunc(keydown);
   glutKeyboardUpFunc(keyup);
   glutIdleFunc(idle);

   //  Create Shader Programs
   shader[0] = CreateComputeProg("dots.comp");
   shader[1] = CreateShaderProg("dots.vert","dots.frag");
   shader[2] = CreateShaderProg("noise.vert","noise.frag");
   shader[3] = CreateGeomProg("lines.vert","lines.geom","lines.frag");

   // Get max work groups
   // int maxWGInvocs = glGetIntegeri(GL_MAX_COMPUTE_WORK_GROUP_INVOCATIONS);
   // int workGroupCounts[3] = { 0 };
   // glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_INVOCATIONS, 0, &workGroupCounts[0]);
   // glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_INVOCATIONS, 1, &workGroupCounts[1]);
   // glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_INVOCATIONS, 2, &workGroupCounts[2]);
   // printf("%d %d %d\n", workGroupCounts[0], workGroupCounts[1], workGroupCounts[2]);

   // Create SSBO
   GLuint ssbo;
   glGenBuffers(1, &ssbo);
   glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo);

   // Create initial buffer data
   DotBuffer_t initialBuffer;
   float rx, ry;
   for (int i=0;i<NUM_DOTS*4;i+=4) {
      rx = (float)rand() / (float)RAND_MAX * 2 - 1;
      ry = (float)rand() / (float)RAND_MAX * 2 - 1;
      initialBuffer.dotPositions[i  ] = rx;
      initialBuffer.dotPositions[i+1] = ry;
      initialBuffer.dotPositions[i+2] = 0;
      initialBuffer.dotPositions[i+3] = 1;

      initialBuffer.prevPositions[i  ] = rx;
      initialBuffer.prevPositions[i+1] = ry;
      initialBuffer.prevPositions[i+2] = 0;
      initialBuffer.prevPositions[i+3] = 1;
   }
   // Generate buffer with data
   glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(DotBuffer_t), &initialBuffer, GL_STATIC_DRAW);
   // Bind buffer to 0 index
   glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, ssbo);

   // Uniform setup
   timeUniformLocation[0] = glGetUniformLocation(shader[0], "time");
   timeUniformLocation[1] = glGetUniformLocation(shader[2], "time");
   deltaTimeUniform = glGetUniformLocation(shader[0],"deltaTime");

   // Bind SSBO to vertex attribute so vertices can be drawn
   // glVertexAttribPointer(0, 4 * NUM_DOTS, GL_FLOAT, GL_FALSE, 0, NULL);
   glEnableVertexAttribArray(0);
   
   //  Pass control to GLUT so it can interact with the user
   ErrCheck("init");
   glutMainLoop();
   return 0;
}

