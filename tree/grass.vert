#version 120

// uniform vec3 LightPos;
// uniform mat4 ModelView;
// uniform mat4 Projection;

varying vec3 vPos;
varying vec3 vNrm;
varying vec3 vCol;
varying vec2 vTex;
varying vec3 vLightPos;

void main() {
    vec4 Pos = gl_ModelViewMatrix * gl_Vertex;
    vec3 lPos = gl_LightSource[0].position.xyz - Pos.xyz;
    gl_Position = gl_ProjectionMatrix * Pos;//Projection * Pos;

    vPos = -Pos.xyz;
    vNrm = gl_NormalMatrix * gl_Normal;
    gl_FrontColor = gl_Color;
    gl_TexCoord[0] = gl_MultiTexCoord0;
    // vTex = texture;
    vLightPos = lPos.xyz - Pos.xyz;
}

