#version 330 core
layout(location = 0) in vec3 vPosition;
layout(location = 1) in vec3 vColor;

uniform mat4 rotationMatrix;

out vec3 color;

void main() {
    gl_Position = rotationMatrix * vec4(vPosition, 1.0);
    color = vColor;
}
