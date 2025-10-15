#version 430 core

// uniform GlobalMatrixBlock {
// } Matrices;
layout(location=0) uniform mat4 u_model;
layout(location=1) uniform mat4 u_view;
layout(location=2) uniform mat4 u_proj;

layout(location=0) in vec3 aPos;
layout(location=1) in vec4 aCol;
layout(location=2) in vec2 aUv;

out vec4 frag_col;
out vec2 frag_uv;

void main() {
    vec4 posM = vec4(aPos, 1.0);
    vec4 p = posM;
    p *= u_model;
    p *= u_view;
    p *= u_proj;

    gl_Position =p;
    frag_col = aCol;
    frag_uv = aUv;
}