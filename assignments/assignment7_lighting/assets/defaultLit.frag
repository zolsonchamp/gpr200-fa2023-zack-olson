#version 450
out vec4 FragColor;

struct Light
{
    vec3 position;
    vec3 color;
};

#define MAX_LIGHTS 4
uniform Light _Lights[MAX_LIGHTS];

in Surface
{
    vec2 UV;
    vec3 WorldPosition;
    vec3 WorldNormal;
} fs_in;

uniform sampler2D _Texture;

void main()
{
    vec3 ambientColor = vec3(0.2, 0.2, 0.2);

    vec3 resultColor = texture(_Texture, fs_in.UV).rgb * ambientColor;

    for (int i = 0; i < MAX_LIGHTS; ++i)
    {
        vec3 lightDir = normalize(_Lights[i].position - fs_in.WorldPosition);

        // Calculate view direction correctly
        vec3 viewDir = normalize(-fs_in.WorldPosition);
        vec3 halfDir = normalize(lightDir + viewDir);

        float diff = max(dot(fs_in.WorldNormal, lightDir), 0.0);

        // Calculate specular reflection
        float spec = pow(max(dot(fs_in.WorldNormal, halfDir), 0.0), 32.0);

        vec3 lightColor = _Lights[i].color * (diff + spec);

        resultColor += lightColor;
    }

    FragColor = vec4(resultColor, 1.0);
}