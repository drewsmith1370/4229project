#version 430

in vec2 ModelPos;
uniform float time;

out vec4 FragColor;

// psrdnoise3.glsl, version 2021-12-02
// Copyright (c) 2021 Stefan Gustavson and Ian McEwan
// (stefan.gustavson@liu.se, ijm567@gmail.com)
// Published under the MIT license, see:
// https://opensource.org/licenses/MIT
vec4 permute(vec4 i) {
vec4 im = mod(i, 289.0);
return mod(((im*34.0)+10.0)*im, 289.0);
}
float psrdnoise(vec3 x, vec3 period, float alpha, out vec3 gradient)
{
    const mat3 M = mat3(0.0,1.0,1.0, 1.0,0.0,1.0, 1.0,1.0,0.0);
    const mat3 Mi = mat3(-0.5,0.5,0.5, 0.5,-0.5,0.5, 0.5,0.5,-0.5);
    // 2. Transform to simplex space and find simplex "base" i0.
    vec3 uvw = M*x;
    vec3 i0 = floor(uvw); vec3 f0 = fract(uvw);
    // 3. Enumerate simplex corners and transform back.
    vec3 g_ = step(f0.xyx, f0.yzz); vec3 l_ = 1.0 - g_;
    vec3 g = vec3(l_.z, g_.xy); vec3 l = vec3(l_.xy, g_.z);
    vec3 o1 = min(g, l); vec3 o2 = max(g, l);
    vec3 i1 = i0 + o1, i2 = i0 + o2, i3 = i0 + 1.0;
    vec3 v0 = Mi*i0, v1 = Mi*i1, v2 = Mi*i2, v3 = Mi*i3;
    // 4. Compute distances to corners before we wrap.
    vec3 x0 = x - v0, x1 = x - v1, x2 = x - v2, x3 = x - v3;
    // 5, 6. Wrap to periods and update i0, i1, i2, i3 accordingly.
    if(any(greaterThan(period, vec3(0.0)))) {
        vec4 vx = vec4(v0.x, v1.x, v2.x, v3.x);
        vec4 vy = vec4(v0.y, v1.y, v2.y, v3.y);
        vec4 vz = vec4(v0.z, v1.z, v2.z, v3.z);
        if(period.x > 0.0) vx = mod(vx, period.x);
        if(period.y > 0.0) vy = mod(vy, period.y);
        if(period.z > 0.0) vz = mod(vz, period.z);
        i0 = floor(M * vec3(vx.x, vy.x, vz.x) + 0.5);
        i1 = floor(M * vec3(vx.y, vy.y, vz.y) + 0.5);
        i2 = floor(M * vec3(vx.z, vy.z, vz.z) + 0.5);
        i3 = floor(M * vec3(vx.w, vy.w, vz.w) + 0.5);
    }
    // 7. Compute hash for each of the four corners.
    vec4 hash = permute( permute( permute(
    vec4(i0.z, i1.z, i2.z, i3.z ))
    + vec4(i0.y, i1.y, i2.y, i3.y ))
    + vec4(i0.x, i1.x, i2.x, i3.x ));
    // 8. Compute rotating gradients using the "well-behaved" method.
    vec4 theta = hash*3.883222077;
    vec4 sz = 0.996539792 - 0.006920415*hash;
    vec4 psi = hash*0.108705628;
    vec4 Ct = cos(theta); vec4 St = sin(theta);
    vec4 sz_prime = sqrt(1.0 - sz*sz);
    vec4 gx, gy, gz;
    // 9b. Rotate with alpha if desired, else take shortcut.
    if(alpha != 0.0) {
        vec4 px = Ct*sz_prime; vec4 py=St*sz_prime;
        vec4 pz = sz;
        vec4 Sp = sin(psi); vec4 Cp = cos(psi);
        vec4 Ctp = St*Sp - Ct*Cp;
        vec4 qx = mix( Ctp*St, Sp, sz);
        vec4 qy = mix(-Ctp*Ct, Cp, sz);
        vec4 qz = -(py*Cp + px*Sp);
        vec4 Sa = vec4(sin(alpha)); vec4 Ca = vec4(cos(alpha));
        gx = Ca*px + Sa*qx; gy = Ca*py + Sa*qy; gz = Ca*pz + Sa*qz;
    } else {
        gx = Ct * sz_prime; gy = St * sz_prime; gz = sz;
    }
    vec3 g0 = vec3(gx.x, gy.x, gz.x), g1 = vec3(gx.y, gy.y, gz.y);
    vec3 g2 = vec3(gx.z, gy.z, gz.z), g3 = vec3(gx.w, gy.w, gz.w);
    // 10. Compute radial falloff.
    vec4 w = 0.5-vec4(dot(x0,x0), dot(x1,x1), dot(x2,x2), dot(x3,x3));
    w = max(w, 0.0); vec4 w2 = w*w; vec4 w3 = w2*w;
    // 11. Linear ramps along gradients (by scalar product)
    vec4 gdotx = vec4(dot(g0,x0), dot(g1,x1), dot(g2,x2), dot(g3,x3));
    // 12, 13. Multiply and sum up the four noise terms.
    float n = dot(w3, gdotx);
    // 14. Compute first-order partial derivatives.
    vec4 dw = -6.0*w2*gdotx;
    vec3 dn0 = w3.x*g0 + dw.x*x0; vec3 dn1 = w3.y*g1 + dw.y*x1;
    vec3 dn2 = w3.z*g2 + dw.z*x2; vec3 dn3 = w3.w*g3 + dw.w*x3;
    gradient = 39.5 * (dn0 + dn1 + dn2 + dn3);
    // Scale noise value to range [-1,1] (empirical factor).
    return 39.5*n;
}

void main() {
    // Set color
    psrdnoise(vec3(ModelPos,time/4),vec3(16),100,FragColor.rgb);
    FragColor.rgb = (FragColor.rrr) / 2 + .5;
    FragColor.a = 1;
}