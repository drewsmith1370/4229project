#version 430

const vec3 urCol = vec3(1,1,0);
const vec3 ulCol = vec3(0,1,1);
const vec3 brCol = vec3(1,0,1);
const vec3 blCol = vec3(1,0,0);

layout(points) in;
layout(line_strip, max_vertices = 3) out;

in VS_OUT {
    vec2 prevPos;
    vec2 prev2;
} gs_in[];
out vec4 gFragColor;
// out vec2 uv;

void main() {
    // set current and prev positions
    vec4 pos = gl_in[0].gl_Position;
    vec2 pp = gs_in[0].prevPos;
    vec2 p2 = gs_in[0].prev2;

    // calc difference and determine if previous should be omitted (otherwise lines would be drawn across screen)
    float dif = abs(pos.x - pp.x) + abs(pos.y-pp.y);
    if (dif > .8) pp = pos.xy;

    dif = abs(pp.x - p2.x) + abs(pp.y-p2.y);
    if (dif > .8) p2 = pp;

    float x = pos.x / 2 + .5;
    float y = pos.y / 2 + .5;
    vec3 interp = urCol * x * y + ulCol * (1-x) * y + brCol * x * (1-y) + blCol * (1-x) * (1-y);
    vec4 fragCol = vec4(interp, 1);

    // Current
    gl_Position = vec4(pos.xy,1,1);
    gFragColor = fragCol;
 // uv = vec2(x,y);
    EmitVertex();

    gl_Position = vec4(pp,0,1);
 // uv = pp.xy / 2 + .5;
    gFragColor = fragCol * .5;
    EmitVertex();

    gl_Position = vec4(p2,0,1);
    gFragColor = vec4(0);
    EmitVertex();

    EndPrimitive();
}