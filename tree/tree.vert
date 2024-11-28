#version 430 core

uniform vec3 LightPos;
uniform mat4 ModelView;
uniform mat4 Projection;

struct Instance {
    mat4 transform;
};

layout(std430, binding=0) buffer Instances
{
    Instance instances[];
};

layout (location=0) in vec3 position;
layout (location=1) in vec3 normal;
layout (location=2) in vec3 color;
layout (location=3) in vec2 texture;

out vec3 vPos;
out vec3 vNrm;
out vec3 vCol;
out vec2 vTex;
out vec3 vLightPos;

void main() {
    mat4 trans = instances[gl_InstanceID].transform;
    if (color.g == 1) trans[3][3] = 1;
    vec4 lPos = ModelView * vec4(LightPos,1);
    vec4 Pos = ModelView * trans * vec4(position,1);
    gl_Position = Projection * Pos;

    vPos = Pos.xyz;
    vNrm = mat3(ModelView) * normal;
    vCol = color;
    vTex = texture;
    vLightPos = lPos.xyz - Pos.xyz;
}
