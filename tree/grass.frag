#version 120

const vec3 Intensity = vec3(0,0,0);

uniform sampler2D tex;

varying vec3 vPos;
varying vec3 vNrm;
varying vec3 vCol;
varying vec2 vTex;
varying vec3 vLightPos;

// Credit to Vlakkies for this shader
void main() {
    vec3 N = normalize(vNrm);
    vec3 L = normalize(vLightPos);
    vec3 R = reflect(-L,N);
    vec3 V = normalize(-vPos);

    float Id = dot(L,N);
    float Is = pow(max(dot(R,V),0),1);

    gl_FragColor = (.1 + .6 * Id + .1 * Is) * gl_Color * texture2D(tex,gl_TexCoord[0].xy);;
}