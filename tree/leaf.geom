#version 430 core

layout (points) in;
layout (triangle_strip, max_vertices=4) out;

in VS_OUT {
    vec3 dir;
    vec3 vNrm;
    vec3 vCol;
    float scale;
} gs_in[];
out vec4 gCol;

void main() {
    // Take inputs
    vec4 pos = gl_in[0].gl_Position;
    vec4 col = vec4(gs_in[0].vCol,1);
    float w = gs_in[0].scale;

    // Find rotation for each leaf
    vec3 dir = gs_in[0].dir;
    vec3 nrm = gs_in[0].vNrm;
    vec3 par = cross(dir,nrm);
    mat4 rot = mat4(vec4(par.x,dir.x,nrm.x,0),
                    vec4(par.y,dir.y,nrm.y,0),
                    vec4(par.z,dir.z,nrm.z,0),
                    vec4(    0,    0,    0,1));



    // Draw Vertices
    // Left
    gl_Position = pos + w * rot * vec4(-.02,.02,0,0);
    gCol = col;
    EmitVertex();

    // Top
    gl_Position = pos + w * rot * vec4(0,.08,0,0);
    gCol = col;
    EmitVertex();

    // Bottom
    gl_Position = pos;
    gCol = col;
    EmitVertex();

    // Right
    gl_Position = pos + w * rot * vec4(.02,.02,0,0);
    gCol = col;
    EmitVertex();

    EndPrimitive();
}