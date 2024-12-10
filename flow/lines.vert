#version 430 core

struct dot_t {
    vec4 pos;
    vec2 vel;
    vec2 prev;
    vec2 prev2;
};

layout (std430, binding = 0) buffer dot_buffer {
    dot_t dots[];
};

layout(location = 0) in vec4 position; // Dot position
layout(location = 1) out vec4 previousPosition; // Previous dot position

out VS_OUT {
    vec2 prevPos;
    vec2 prev2;
} vs_out;

void main() {
    // Get the index of the vertex for the arrays
    uint vertIdx = gl_VertexID;
    vec4 vpos = dots[vertIdx].pos;
    // Convert to screen coordinates
    gl_Position = vec4(vpos.xy * 2 - 1, 0,1);
    // Pass previous position for geom shader
    vs_out.prevPos = vec2(dots[vertIdx].prev.xy * 2 - 1);
    vs_out.prev2 = vec2(dots[vertIdx].prev2.xy * 2 - 1);
}