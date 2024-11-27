#version 430 core

uniform vec3 LightPos;
uniform mat4 ModelView;
uniform mat4 Projection;

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
    vec4 lPos = ModelView * vec4(LightPos,1);
    vec4 Pos = ModelView * vec4(position,1);
    gl_Position = Projection * Pos;

    vPos = Pos.xyz;
    vNrm = mat3(ModelView) * normal;
    vCol = color;
    vTex = texture;
    vLightPos = lPos.xyz - Pos.xyz;
}

// void main()
// {
//    //  Vertex location in modelview coordinates
//    vec4 P = gl_ModelViewMatrix * gl_Vertex;
//    //  Light position
//    Light  = gl_LightSource[0].position.xyz - P.xyz;
//    //  Normal
//    Normal = gl_NormalMatrix * gl_Normal;
//    //  Eye position
//    View  = -P.xyz;
//    //  Texture
//    gl_TexCoord[0] = gl_MultiTexCoord0;
//    //  Set vertex position
//    gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
// }
