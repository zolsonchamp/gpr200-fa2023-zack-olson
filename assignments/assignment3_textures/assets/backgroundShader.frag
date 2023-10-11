#version 450
out vec4 FragColor;
in vec2 UV;

uniform sampler2D _Texture;

void main() {
    vec2 distortedTexCoord = UV + vec2(sin(UV.y * 10.0), cos(UV.x * 10.0)) * 0.02;

    vec2 refractedTexCoord = distortedTexCoord;

    vec4 texColor = texture(_Texture, refractedTexCoord);

    texColor.rgb *= 0.7;
    
    FragColor = texColor;
}