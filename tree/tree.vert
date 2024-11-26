#version 430 core

uniform vec4 Light;

layout (location=0) in vec3 aPosition;
layout (location=1) in vec3 aNormal;
layout (location=2) in vec3 aColor;
layout (location=3) in vec2 aTexture;

out vec3 vPos;
out vec3 vNrm;
out vec3 vCol;
out vec2 vTex;

void main() {
    gl_Position = vec4(aPosition,1);
    vPos = aPosition;
    vNrm = aNormal;
    vCol = aColor;
    vTex = aTexture;
}