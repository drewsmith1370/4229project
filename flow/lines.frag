#version 430 core

uniform sampler2D fboTex;

in vec2 uv;
in vec4 gFragColor;
out vec4 FragColor;

void main() {
    // vec4 currColor = gFragColor;
    // vec4 decayColor = texture(fboTex, uv);
    // FragColor = vec4(currColor.rgb + decayColor.rgb, 1);

    FragColor = gFragColor;
}