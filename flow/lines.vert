#version 430 core

layout (std430, binding = 0) buffer dot_buffer {
    vec4 pos  [131072];
    vec2 vel  [131072];
    vec4 prev [131072];
};

layout(location = 0) in vec4 position; // Dot position
layout(location = 1) out vec4 previousPosition; // Previous dot position

out VS_OUT {
    vec4 prevPos;
} vs_out;

void main() {
    // Get the index of the vertex for the arrays
    uint vertIdx = gl_VertexID;
    vec4 vpos = pos[vertIdx];
    // Convert to screen coordinates
    gl_Position = vec4(vpos.xy * 2 - 1, 0,1);
    // Pass previous position for geom shader
    vs_out.prevPos = vec4(prev[vertIdx].xy * 2 - 1, 0,1);
}