#version 330 core
layout (location = 0) in vec2 aPos;

void main() {
    // Vi sender bare koordinaterne direkte igennem
    gl_Position = vec4(aPos, 0.0, 1.0);
}