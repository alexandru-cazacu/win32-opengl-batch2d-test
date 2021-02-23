#if defined(_TYPE_VERTEX_SHADER)

layout (location = 0) in vec3 a_Pos;
layout (location = 1) in vec3 a_Normal;
layout (location = 2) in vec4 a_Color;

out vec4 v_VertexColor;
out vec3 v_FragPos;
out vec3 v_Normal;

uniform mat4 u_Model;
uniform mat4 u_ViewProj;

void main()
{
    v_VertexColor = a_Color;
    v_Normal = a_Normal;
    v_FragPos = vec3(u_Model * vec4(a_Pos, 1.0));
    gl_Position = u_ViewProj * vec4(a_Pos, 1.0);
};

#elif defined(_TYPE_FRAGMENT_SHADER)

struct Material
{
    vec4 ambient;
    vec4 diffuse;
    vec4 specular;
    float shininess;
};

struct Light {
    vec3 position;
  
    vec4 ambient;
    vec4 diffuse;
    vec4 specular;
};

in vec4 v_VertexColor;
in vec3 v_FragPos;
in vec3 v_Normal;

layout (location = 0) out vec4 o_FragColor;

uniform vec4 u_LightColor;
uniform vec3 u_ViewPos;

uniform Material u_Material;
uniform Light u_Light;

void main()
{
    // Ambient
    vec4 ambient = u_Light.ambient * u_Material.ambient;

    // TODO alex Add normal model via uniform. 3x3 inverse and transpose.
    // Diffuse
    vec3 norm = normalize(v_Normal);
    vec3 lightDir = normalize(u_Light.position - v_FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec4 diffuse = u_Light.diffuse * (diff * u_Material.diffuse);

    // Specular
    vec3 viewDir = normalize(u_ViewPos - v_FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), u_Material.shininess);
    vec4 specular = u_Light.specular * (spec * u_Material.specular);

    o_FragColor = ambient + diffuse + specular;
}

#endif
