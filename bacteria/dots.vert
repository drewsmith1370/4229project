#version 430

layout (std430, binding = 0) buffer dot_buffer {
    vec4 pos[];
};

out vec4 Color;

uniform vec3 c1;
uniform vec3 c2;

void main() {
    uint vertexIndex = gl_VertexID;
    vec2 vert = pos[vertexIndex].xy;
    gl_Position = vec4(vert*2 - 1,0,1);
    //Color = vec4(c1 * vert.x + c2*vert.y + c3*(1-vert.x*vert.y),1);
    Color = vec4(
        float(gl_VertexID % 2)*c1.x + float(abs(gl_VertexID % 2 - 1))*c2.x, 
        float(gl_VertexID % 2)*c1.y + float(abs(gl_VertexID % 2 - 1))*c2.y,
        float(gl_VertexID % 2)*c1.z + float(abs(gl_VertexID % 2 - 1))*c2.z,
        1);
}