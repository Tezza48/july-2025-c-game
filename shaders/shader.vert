#version 330 core

void main() {
    // Draws a triangle based on vertex index
    vec3 positions[3] = vec3[](
        vec3(-0.5, -0.5, 0.0), // Bottom left
        vec3(0.5, -0.5, 0.0),  // Bottom right
        vec3(0.0, 0.5, 0.0)    // Top
    );

    gl_Position = vec4(positions[gl_VertexID], 1.0);
}