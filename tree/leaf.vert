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

out VS_OUT {
    vec3 dir;
    vec3 vNrm;
    vec3 vCol;
    float scale;
} vs_out;

void main() {
    mat4 trans = instances[gl_InstanceID].transform;
    vec4 lPos = ModelView * vec4(LightPos,1);
    vec4 Pos = ModelView * trans * vec4(position,1);

    // Outputs
    gl_Position = Projection * Pos;
    vs_out.vNrm = mat3(ModelView) * mat3(trans) * normal;
    // vs_out.vCol = color;
    vs_out.dir = mat3(Projection) * mat3(ModelView) * mat3(trans) * vec3(0,1,0);
    vs_out.scale = ModelView[3][3] * trans[3][3];

    vec3 N = normalize(mat3(ModelView) * mat3(trans) * normal);
    vec3 L = normalize(LightPos - position);
    vec3 R = reflect(-L,N);
    vec3 V = normalize(-position);

    float Id = dot(L,N) / 2 + .5;
    float Is = pow(max(dot(R,V),0),1);

    vs_out.vCol = (.2 + .6 * Id + .2 * Is) * color;
}
