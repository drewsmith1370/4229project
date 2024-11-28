#version 430 core

uniform vec3 Intensity;

in vec3 vPos;
in vec3 vNrm;
in vec3 vCol;
in vec2 vTex;
in vec3 vLightPos;

out vec4 FragColor;

void main() {
    vec3 N = normalize(vNrm);
    vec3 L = normalize(vLightPos);
    vec3 R = reflect(-L,N);
    vec3 V = normalize(-vPos);

    float Id = dot(L,N) / 2 + .5;
    float Is = pow(max(dot(R,V),0),1);

    FragColor = (.2 + .6 * Id + .2 * Is) * vec4(vCol,1);
}