	#version 450
	layout(location = 0) in vec3 vPos;
	in vec2 vUV;
	out vec2 UV;
	out vec2 fragCoord;
	void main(){
		UV = vUV;
		fragCoord = vec2(vPos.x, vPos.y);
		gl_Position = vec4(vPos,1.0);
		
	}