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

    FragColor = (.1 + .7 * Id + .2 * Is) * vec4(vCol,1);
}


// varying vec3 View;
// varying vec3 Light;
// varying vec3 Normal;

// void main()
// {
//    //  N is the object normal
//    vec3 N = normalize(Normal);
//    //  L is the light vector
//    vec3 L = normalize(Light);
//    //  R is the reflected light vector R = 2(L.N)N - L
//    vec3 R = reflect(-L,N);
//    //  V is the view vector (eye vector)
//    vec3 V = normalize(View);

//    //  Diffuse light is cosine of light and normal vectors
//    float Id = max(dot(L,N) , 0.0);
//    //  Specular is cosine of reflected and view vectors
//    float Is = (Id>0.0) ? pow(max(dot(R,V),0.0) , gl_FrontMaterial.shininess) : 0.0;

//    //  Sum color types
//    gl_FragColor = gl_FrontMaterial.emission
//                 + gl_FrontLightProduct[0].ambient
//                 + Id*gl_FrontLightProduct[0].diffuse
//                 + Is*gl_FrontLightProduct[0].specular;
// }
