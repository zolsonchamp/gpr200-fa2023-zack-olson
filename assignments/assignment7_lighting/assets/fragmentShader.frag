#version 450
out vec4 FragColor;

in Surface{
	vec3 NormalOS; //Normal in Object/Model space
	vec2 UV;
}fs_in;

uniform sampler2D _Texture;

void main(){
	vec3 normal = normalize(fs_in.NormalOS); //Re-normalize after interpolation
	FragColor = vec4(abs(normal),1.0);
}