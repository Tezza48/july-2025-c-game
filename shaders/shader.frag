#version 330 core
// Output a red for each fragment

layout(location = 0) out vec4 fragColor;

// Main function for fragment shader
void main() {
    // Set the output color to red
    fragColor = vec4(0.9, 0.1, 0.1, 1.0);
}