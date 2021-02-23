#if defined(_TYPE_VERTEX_SHADER)

layout (location = 0) in vec3 a_Pos;
layout (location = 1) in vec3 a_Color;
layout (location = 2) in vec2 a_TexCoord;

out vec4 v_VertexColor;
out vec2 v_TexCoord;

uniform mat4 u_ViewProj;
uniform mat4 u_Transform;

void main()
{
    v_VertexColor = vec4(a_Color, 1.0);
    v_TexCoord = a_TexCoord;
    gl_Position = u_ViewProj * u_Transform * vec4(a_Pos, 1.0);
};

#elif defined(_TYPE_FRAGMENT_SHADER)

in vec4 v_VertexColor;
in vec2 v_TexCoord;

out vec4 o_FragColor;

uniform sampler2D u_Texture;
uniform vec4 u_Color;

void main()
{
    o_FragColor = texture(u_Texture, v_TexCoord) * v_VertexColor * u_Color;
}

#endif
