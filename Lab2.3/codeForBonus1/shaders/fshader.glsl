#version 330 core

in vec4 color;
out vec4 fColor;
uniform float darkness;
void main()
{
    fColor =color*darkness; 
}

