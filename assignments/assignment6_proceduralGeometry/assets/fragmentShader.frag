#version 450
out vec4 FragColor;

in vec3 Normal;
in vec2 UV;
uniform sampler2D _Texture;

//0 for normals
//1 for UVs
//2 for texture
uniform int _Mode;

void main(){
	if (_Mode == 0){
		FragColor = vec4(abs(Normal),1.0);
	}else if (_Mode == 1){
		FragColor = vec4(UV,0.0,1.0);
	}
	else if (_Mode == 2){
		FragColor = texture(_Texture,UV);
	}
}