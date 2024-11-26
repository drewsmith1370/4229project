#version 430 core

uniform vec4 Light;

in vec3 vPos;
in vec3 vNrm;
in vec3 vCol;
in vec2 vTex;

out vec4 FragColor;

void main() {
    FragColor = vec4(vPos,1);
}