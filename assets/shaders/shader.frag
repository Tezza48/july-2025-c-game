#version 430 core
// Output a red for each fragment

layout(location = 0) out vec4 oCol;


uniform sampler2D tex;


in vec4 frag_col;
in vec2 frag_uv;

// Main function for fragment shader
void main() {
    // Set the output color to red
    oCol = frag_col;
    oCol = texture(tex, frag_uv) * frag_col;
}