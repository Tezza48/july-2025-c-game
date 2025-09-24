#version 330 core
// Output a red for each fragment

layout(location = 0) out vec4 oCol;


in vec4 frag_col;

// Main function for fragment shader
void main() {
    // Set the output color to red
    oCol = frag_col;
}