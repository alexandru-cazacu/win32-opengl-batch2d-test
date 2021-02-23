#version 330 core

in vec2 o_TexCoords;
  
out vec4 o_FragColor;

uniform sampler2D u_FrameBufferTexture;

void main()
{ 
    o_FragColor = texture(u_FrameBufferTexture, o_TexCoords);
}