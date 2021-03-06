#version 330 core

in vec4 v_VertexColor;
in vec2 v_TexCoord;
in float v_TexIndex;

layout (location = 0) out vec4 o_FragColor;

uniform sampler2D u_Textures[32];

void main()
{
    int index = int(v_TexIndex);
    o_FragColor = texture(u_Textures[index], v_TexCoord) * v_VertexColor;
};
