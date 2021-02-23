#version 330 core

in vec3 o_Color;

out vec4 FragColor;  
  
void main()
{
    FragColor = vec4(o_Color, 1.0);
}