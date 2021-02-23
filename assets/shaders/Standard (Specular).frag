#version 330 core

in vec2 v_TexCoords;
in vec3 v_Color;
  
out vec4 o_FragColor;

uniform sampler2D u_DiffuseMap;
uniform sampler2D u_SpecularMap;
uniform vec4 u_DiffuseColor;

void main()
{ 
    o_FragColor = u_DiffuseColor * texture(u_DiffuseMap, v_TexCoords) * texture(u_SpecularMap, v_TexCoords);
}