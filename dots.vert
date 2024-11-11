#version 430

layout (std430, binding = 0) buffer dot_buffer {
    vec4 pos[];
};

out vec4 Color;

const vec3 c1 = vec3(1,0,0);
const vec3 c2 = vec3(0,0,1);
const vec3 c3 = vec3(0,1,0);

void main() {
    uint vertexIndex = gl_VertexID;
    vec2 vert = pos[vertexIndex].xy;
    gl_Position = vec4(vert*2 - 1,0,1);
    Color = vec4(c1 * vert.x + c2*vert.y + c3*(1-vert.x*vert.y),1);
}