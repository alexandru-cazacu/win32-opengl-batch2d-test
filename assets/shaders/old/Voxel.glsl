#if defined(_TYPE_VERTEX_SHADER)

layout (location = 0) in vec3 a_Pos;
layout (location = 1) in vec3 a_Normal;
layout (location = 2) in vec4 a_Color;
layout (location = 3) in float a_AmbientOcclusion;

out vec4 v_VertexColor;
out float v_AmbientOcclusion;

uniform mat4 u_Transform;
uniform mat4 u_ViewProj;

void main()
{
    v_VertexColor = a_Color;
    v_AmbientOcclusion = a_AmbientOcclusion;
    gl_Position = u_ViewProj * u_Transform * vec4(a_Pos, 1.0);
};

#elif defined(_TYPE_FRAGMENT_SHADER)

struct Light {
    vec4 ambient;
};

in vec4 v_VertexColor;
in float v_AmbientOcclusion;

layout (location = 0) out vec4 o_FragColor;

uniform Light u_Light;

void main()
{
    o_FragColor = u_Light.ambient * v_VertexColor * vec4(v_AmbientOcclusion, v_AmbientOcclusion, v_AmbientOcclusion, 1);
}

#endif
