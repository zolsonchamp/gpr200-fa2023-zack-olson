	#version 450
	out vec4 fragColor;
    in vec2 fragCoord; 
	in vec2 UV;

    uniform vec3 iResolution;
    uniform float iTime;
    uniform vec3 skyColor;

	void main()
{
    // Normalized pixel coordinates (from 0 to 1)
    vec2 uv = fragCoord + 0.5 / 2;

    // Time varying pixel color
    vec3 col = 0.5 + 0.5*cos(iTime+uv.xyx+skyColor);

    vec2 sunUV = uv * 2.0 - 1.0;
    sunUV.x *= iResolution.x / iResolution.y;
    float d = distance(sunUV, vec2(0.0, sin(iTime)));
    d = smoothstep(0.5,0.6,d);
    col = mix(vec3(1.0,1.0,0.0),col,d);
    
    float y = sin(uv.x*2.0)*0.4;
    float t = step(y,uv.y);
    col = mix(vec3(0.0,0.3,0.0),col,t);


    // Output to screen
    fragColor = vec4(col,1.0);
}