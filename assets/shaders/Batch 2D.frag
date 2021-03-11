#version 330 core

in vec4 v_VertexColor;
in vec2 v_TexCoord;
in float v_TexIndex;

layout (location = 0) out vec4 o_FragColor;

uniform sampler2D u_Textures[32];

float median(float r, float g, float b) {
    return max(min(r, g), min(max(r, g), b));
}

float screenPxRange() {
	float pxRange = 2.0;

    int index = int(v_TexIndex);
    vec2 unitRange = vec2(pxRange) / vec2(textureSize(u_Textures[index], 0));
    vec2 screenTexSize = vec2(1.0) / fwidth(v_TexCoord);
    return max(0.5 * dot(unitRange, screenTexSize), 1.0);
}

void main()
{
    int index = int(v_TexIndex);
    vec3 msd = texture(u_Textures[index], v_TexCoord).rgb;
    float sd = median(msd.r, msd.g, msd.b);
    float screenPxDistance = screenPxRange()*(sd - 0.5);
    float opacity = clamp(screenPxDistance + 0.5, 0.0, 1.0);

	vec4 bgColor = vec4(0.0f, 0.0f, 0.0f, 0.0f);
	vec4 fgColor = vec4(1.0f, 1.0f, 1.0f, 1.0f);

    o_FragColor = mix(bgColor, fgColor, opacity);
};
