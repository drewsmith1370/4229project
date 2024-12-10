#include "CSCIx229.h"
#define PARTICLE_MODE 0
#define DOT_MODE 1
#define SIZE_WORKGROUP 1024
#define NUM_INVOCATIONS 128
#define NUM_DOTS SIZE_WORKGROUP*NUM_INVOCATIONS
// #define RES 16/9

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
int shader[]  = {0,0,0,0,0}; // Shaders
unsigned int noise;
unsigned int timeUniformLocation[2];
unsigned int deltaTimeUniform;
unsigned int texUniformLoc;
unsigned int color1Location;
unsigned int color2Location;
unsigned int texShowLoc;
// FBOs
unsigned int texs[2];
unsigned int FBOs[2];
// Time
bool moveTime = true;
double progTime = 0;
double deltaTime = 0;
int timecheck = 0;
// Window
double asp=1;
double dim=1;
bool keys[256];
// Controls
int mode = !PARTICLE_MODE;
bool showingField = false;
// Texture
GLuint textureID = 0;
GLuint textureIDs[10];
GLuint VAO = 0;
GLuint VBO = 0;
// Colors
float color1[3];
float color2[3];

void printTexture(float* textureData, int numPix)
{
 for (int i=0;i<4*numPix/10000;i+=4) {
   printf("%.01f,%.01f,%.01f,%.01f\n",textureData[i],textureData[i+1],textureData[i+2],textureData[i+3]);
 }
}

GLuint loadTextureFromFile(int width, int height) {

    // Step 2: Allocate memory for texture data
    int texelCount = width * height * 4;  // 4 components per texel (RGBA)
    float *textureData = (float *)malloc(texelCount * sizeof(float));

   float center1x = (float)(rand() % 1001 - 500) / (float)(500);
   float center1y = (float)(rand() % 1001 - 500) / (float)(500);
   float center2x = (float)(rand() % 1001 - 500) / (float)(500);
   float center2y = (float)(rand() % 1001 - 500) / (float)(500);
   float center3x = (float)(rand() % 1001 - 500) / (float)(500);
   float center3y = (float)(rand() % 1001 - 500) / (float)(500);

   float dx = (float)2 / (float)width;
   float dy = (float)2 / (float)height;
   float x = -1;
   float y = -1;
   float val = 0;
   float radius = 0.4;
   float xd = 0;
   float yd = 0;

   for(int i = 0; i < width; i++)
   {
      for(int j = 0; j < width; j++)
      {
         val = 0;

         xd = (x-center1x);
         yd = (y-center1y);
         val += exp(-(xd * xd + yd * yd) / (2.0f * radius * radius));

         xd = (x-center2x);
         yd = (y-center2y);
         val += exp(-(xd * xd + yd * yd) / (2.0f * radius * radius));

         xd = (x-center3x);
         yd = (y-center3y);
         val += exp(-(xd * xd + yd * yd) / (2.0f * radius * radius));

         textureData[i] = val;
         textureData[i+1] = 0;
         textureData[i+2] = 0;
         textureData[i+3] = 0;

         y += dy;
      }
      x += dx;
   }


   //printTexture(textureData,4*width*height);

    // Step 4: Create OpenGL texture
    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);

    // Step 5: Upload the texture data to OpenGL
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, width, height, 0, GL_RGBA, GL_FLOAT, textureData);

    // Step 6: Set texture parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glBindTexture(GL_TEXTURE_2D, 0);  // Unbind the texture

    // Step 7: Free allocated memory
    free(textureData);

    return textureID;
}

void initializeTextures() {

   for(int i = 0; i < 10; i++)
   {
    textureIDs[i] = loadTextureFromFile(500, 500);
    if (textureIDs[i] == 0) {
        fprintf(stderr, "Failed to load texture.\n");
    }
   }
}

// Function to switch between textures
void bindActiveTexture() {
   int id = timecheck % 10;
   glBindTexture(GL_TEXTURE_2D, textureIDs[id]);
   textureID = textureIDs[id];
}

/*
 *  Display Function
 */
void display() {
   glClear(GL_COLOR_BUFFER_BIT);

   // Build chemical background textures

   if(floor(progTime/2) > timecheck || progTime == 0)
   {
      if(progTime != 0)
      {
         timecheck++;
      }
      bindActiveTexture();
      float pert1 = (float)(rand()%1000-500)/10000.0f;
      float pert2 = (float)(rand()%1000-500)/10000.0f;
      float pert3 = (float)(rand()%1000-500)/10000.0f;
      color1[0] += pert1;
      color1[1] += pert2;
      color1[2] += pert3;
      color2[0] = color1[0];
      color2[1] = color1[1];
      color2[2] = color1[2];
   }

   glPointSize(3);
   glLoadIdentity();
   glLineWidth(2);

   // Compute shader to determine dot positions
   glUseProgram(shader[0]);
   glUniform1f(timeUniformLocation[0], (float)progTime);
   glUniform1f(deltaTimeUniform, deltaTime);
   glBindTextureUnit(1, textureID);
   glUniform1i(texUniformLoc,1);
   glDispatchCompute(NUM_INVOCATIONS,1,1);
   glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

   if (mode == PARTICLE_MODE)
   {
      // Render program to display dots
      glUseProgram(shader[3]);
      //glUniform3fv(color1Location, 1, color1);
      //glUniform3fv(color2Location, 1, color2);
      glDrawArrays(GL_POINTS,0,NUM_DOTS);
   }
   else {
      glUseProgram(shader[1]);
      glUniform3fv(color1Location, 1, color1);
      glUniform3fv(color2Location, 1, color2);
      glDrawArrays(GL_POINTS,0,NUM_DOTS);
   }
   if (showingField) {
      glUseProgram(shader[4]);

      // Bind texture
      glActiveTexture(GL_TEXTURE0);
      glBindTexture(GL_TEXTURE_2D, textureID);

      // Draw fullscreen quad
      glBindVertexArray(VAO);
      glDrawArrays(GL_TRIANGLES, 0, 6);
   }

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
      //glUniform3fv(color1Location, 1, color1);
      //glUniform3fv(color2Location, 1, color2);
      // glBindTexture(GL_TEXTURE_2D, texs[0]);
      glDrawArrays(GL_POINTS,0,NUM_DOTS);
   }
   else {
      glUseProgram(shader[1]);
      glUniform3fv(color1Location, 1, color1);
      glUniform3fv(color2Location, 1, color2);
      glDrawArrays(GL_POINTS,0,NUM_DOTS);
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
   FILE* f = fopen(file,"rb");
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
   //  Set the viewport to the entire window
   glViewport(0,0, RES*width,RES*height);
   //  Tell OpenGL we want to manipulate the projection matrix
   // glMatrixMode(GL_PROJECTION);
   // //  Undo previous transformations
   // glLoadIdentity();
   //  Orthogonal projection box adjusted for the
   //  aspect ratio of the window
   asp = (height>0) ? (double)width/height : 1;
   //  Switch to manipulating the model matrix
   // glMatrixMode(GL_MODELVIEW);
   Project(0, asp, dim);
   //  Undo previous transformations
   glLoadIdentity();
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

// void generatePingPongFBOs(int width, int height, unsigned int textures[2], unsigned int fbos[2]) {
//    // Create textures
//    glGenTextures(2, textures);
//    // Initialize texture parameters
//    for (int i = 0; i < 2; i++) {
//       glBindTexture(GL_TEXTURE_2D, textures[i]);
//       glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, width, height, 0, GL_RGBA, GL_FLOAT, NULL);
//       glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
//       glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
//       glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
//       glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
//    }

//    // Generate frame buffers
//    glGenFramebuffers(2, fbos);

//    // Attach textures to FBOs
//    for (int i = 0; i < 2; i++) {
//       glBindFramebuffer(GL_FRAMEBUFFER, fbos[i]);
//       // Bind 
//       glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textures[i], 0);
//       GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
//       if (status != GL_FRAMEBUFFER_COMPLETE) {
//          Fatal("FBO incomplete\n");
//       }
//    }
// }

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

   // Initialize texture
   initializeTextures();

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
   shader[4] = CreateShaderProg("tex.vert", "tex.frag");

   // Get max work groups
   // int maxWGInvocs = glGetIntegeri(GL_MAX_COMPUTE_WORK_GROUP_INVOCATIONS);
   // int workGroupCounts[3] = { 0 };
   // glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_INVOCATIONS, 0, &workGroupCounts[0]);
   // glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_INVOCATIONS, 1, &workGroupCounts[1]);
   // glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_INVOCATIONS, 2, &workGroupCounts[2]);
   // printf("%d %d %d\n", workGroupCounts[0], workGroupCounts[1], workGroupCounts[2]);

   glGenVertexArrays(1, &VAO);
   glGenBuffers(1, &VBO);

   glBindVertexArray(VAO);

   float vertices[] = {
      // Positions   // Texture Coords
      -1.0f,  1.0f,   0.0f, 1.0f, // Top-left
      -1.0f, -1.0f,   0.0f, 0.0f, // Bottom-left
      1.0f, -1.0f,   1.0f, 0.0f, // Bottom-right

      -1.0f,  1.0f,   0.0f, 1.0f, // Top-left
      1.0f, -1.0f,   1.0f, 0.0f, // Bottom-right
      1.0f,  1.0f,   1.0f, 1.0f  // Top-right
   };

   glBindBuffer(GL_ARRAY_BUFFER, VBO);
   glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

   // Position attribute
   glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
   glEnableVertexAttribArray(0);

   // Texture coordinate attribute
   glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
   glEnableVertexAttribArray(1);

   // Create SSBO
   GLuint ssbo;
   glGenBuffers(1, &ssbo);
   glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo);

   // Create initial buffer data
   DotBuffer_t* initialBuffer = malloc(sizeof(DotBuffer_t));
   float rx, ry;
   for (int i=0;i<NUM_DOTS*4;i+=4) {
      rx = (float)rand() / (float)RAND_MAX * 2 - 1;
      ry = (float)rand() / (float)RAND_MAX * 2 - 1;
      
      initialBuffer->dotPositions[i  ] = rx;
      initialBuffer->dotPositions[i+1] = ry;
      initialBuffer->dotPositions[i+2] = 0;
      initialBuffer->dotPositions[i+3] = 1;

      initialBuffer->prevPositions[i  ] = rx;
      initialBuffer->prevPositions[i+1] = ry;
      initialBuffer->prevPositions[i+2] = 0;
      initialBuffer->prevPositions[i+3] = 1;
   }

   // Generate buffer with data
   glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(DotBuffer_t), initialBuffer, GL_STATIC_DRAW);
   // Bind buffer to 0 index
   glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, ssbo);
   free(initialBuffer);

   // Uniform setup
   timeUniformLocation[0] = glGetUniformLocation(shader[0], "time");
   timeUniformLocation[1] = glGetUniformLocation(shader[4], "time");
   deltaTimeUniform = glGetUniformLocation(shader[0],"deltaTime");
   texUniformLoc = glGetUniformLocation(shader[0],"inTexture");
   color1Location = glGetUniformLocation(shader[1], "c1");
   color2Location = glGetUniformLocation(shader[1], "c2");
   texShowLoc = glGetUniformLocation(shader[4],"texture1");

   color1[0] = (float)(rand()%1000)/1000.0f;
   color1[1] = (float)(rand()%1000)/1000.0f;
   color1[2] = (float)(rand()%1000)/1000.0f;


   // Bind SSBO to vertex attribute so vertices can be drawn
   // glVertexAttribPointer(0, 4 * NUM_DOTS, GL_FLOAT, GL_FALSE, 0, NULL);
   glEnableVertexAttribArray(0);
   
   //  Pass control to GLUT so it can interact with the user
   ErrCheck("init");
   glutMainLoop();
   return 0;
}

