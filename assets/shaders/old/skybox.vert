#version 330 core
layout (location = 0) in vec2 aPos;
  
out vec3 o_Color;

uniform mat4 u_Model;
uniform mat4 u_View;
uniform mat4 u_Projection;

uniform vec3 u_viewVec;
uniform vec3 u_topColor;
uniform vec3 u_bottomColor;
uniform float u_Spread;

void main()
{
    gl_Position = u_Projection * u_View * u_Model * vec4(aPos, 1.0, 1.0);

    vec3 topColorHi = u_topColor * clamp((((u_viewVec.x + 90.0) / 180) + u_Spread), 0.0, 1.0);
    vec3 topColorLo = u_topColor * clamp((((u_viewVec.x + 90.0) / 180) - u_Spread), 0.0, 1.0);
    vec3 botColorHi = u_bottomColor * clamp(((1 - (u_viewVec.x + 90.0) / 180) - u_Spread), 0.0, 1.0);
    vec3 botColorLo = u_bottomColor * clamp(((1 - (u_viewVec.x + 90.0) / 180) + u_Spread), 0.0, 1.0);

    // Top
    if (gl_VertexID == 2 || gl_VertexID == 3)
    {
        o_Color = topColorHi + botColorHi;
    }
    // Bottom
    else
    {
        o_Color = topColorLo + botColorLo;
    }
}  