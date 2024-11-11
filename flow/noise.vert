#version 430



in vec4 Vertex;
out vec2 ModelPos;

void main() {
    //  Save model coordinates (for fragment shader)
   ModelPos = Vertex.xy / 2 + .5;
   //  Return fixed transform coordinates for this vertex
   gl_Position = Vertex;
}