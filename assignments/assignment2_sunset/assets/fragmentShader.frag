#version 330 core

// Uniform variables
uniform float u_time;
uniform float fracture;
uniform float timeMultiplier;
uniform vec2 u_resolution;
uniform vec4 portalColor;
uniform vec4 glowColor;
uniform int setColor;
uniform vec2 portalPos;

// Noise generation

// Function to calculate the modulus of a 4D vector with 289.0
vec4 mod289(vec4 x) {
    return x - floor(x * (1.0 / 289.0)) * vec4(289.0);
}

// Function to calculate the modulus of a 3D vector with 289.0
vec3 mod289(vec3 x) {
    return x - floor(x * (1.0 / 289.0)) * vec3(289.0);
}

// Function to calculate the gradient for 4D simplex noise
vec4 grad4(float j, vec4 ip) {
    const vec4 ones = vec4(1.0, 1.0, 1.0, -1.0);
    vec4 p, s;

    // Calculate the integer part of the input vector times 7.0
    // Multiply with the input vector and subtract 1.0
    p.xyz = floor(fract(vec3(j) * ip.xyz) * 7.0) * ip.z - 1.0;
    
    // Calculate the dot product of the absolute value of p.xyz and ones.xyz
    p.w = 1.5 - dot(abs(p.xyz), ones.xyz);
    
    // Create a vector indicating which components of p are less than 0.0
    s = vec4(lessThan(p, vec4(0.0)));

    // Return the modified p vector based on the conditions
    return p - s * (s * p.xxyz + s.yzwx);
}

// Main simplex noise function
float snoise(vec3 x) {
    const vec3 C = vec3(1.0 / 6.0, 1.0 / 3.0, 1.0 / 2.0);
    const vec4 D = vec4(0.0, 0.5, 1.0, 2.0);

    // Adjust the scale of the input vector
    x *= 0.5;

    // Calculate the integer and fractional parts of the input vector
    vec3 i = floor(x + dot(x, C.yyy));
    vec3 t = floor(i * C.yyy) + dot(i, D.yzw);

    // Calculate three vectors x0, x1, and x2 based on the input vector and constants
    vec3 x0 = x - i + dot(i, C.xxx);
    vec3 g = step(x0.yzx, x0.xyz);

    vec3 l = 1.0 - g;
    vec3 i1 = min(g.xyz, l.zxy);
    vec3 i2 = max(g.xyz, l.zxy);

    vec3 x1 = x0 - i1 + C.xxx;
    vec3 x2 = x0 - i2 + C.yyy;
    vec3 x3 = x0 - D.yyy;

    // Calculate the gradient for the four corners of the simplex in 4D space
    i = mod289(i);
    vec4 p = grad4(dot(i, D.yzw), vec4(t, 0.0));

    // Calculate the contributions from each corner to the final noise value
    float n0 = 0.5 - x0.x * x0.x - x0.y * x0.y - x0.z * x0.z;
    float n1, n2, n3;

    n0 = max(n0, 0.0);
    n1 = max(dot(x1, p.xyz), 0.0);
    n2 = max(dot(x2, p.yzw), 0.0);
    n3 = max(dot(x3, p.wyz), 0.0);

    // Combine and scale the contributions to get the final noise value
    return 2.2 * (n0 + n1 + n2 + n3);
}

// Function to create a circular pattern using length with added distortion
float CreateCircle(vec2 position, float radius, float distortionStrength) {
    // Calculate the displacement vector from the center
    vec2 d = position - vec2(0.5);
    
    // Correct for aspect ratio
    d.x *= u_resolution.x / u_resolution.y;

    // Add Perlin noise-based distortion
    float distortion = distortionStrength * snoise(vec3(position * 5.0, fracture));
    d += distortion;

    // Use step function to create a circular shape
    return 1.0 - step(radius, length(d));
}

void main() {

    // Calculate normalized coordinates for the current fragment
    vec2 coord = (gl_FragCoord.xy / u_resolution);
    
    // Shift the y-coordinate to create a vertical offset
    coord.y -= 0.25f;
    
    // Initialize color as black
    vec3 color = vec3(0.0);

    // Calculate angle based on the modified coordinates
    float angle = atan(-coord.y + 0.25, coord.x - 0.5) * 0.1;
    
    // Calculate the distance from the center
    float len = length(coord - vec2(0.5, 0.25));

    // Add glow effect
    float glow = smoothstep(0.5, 0.35, len);

    switch(setColor)
    {
        case 0:
            // Generate swirl with manual color and glow
            color.r += sin(len * 40.0 + angle * 40.0 + u_time * timeMultiplier) * portalColor[0];
            color.g += sin(len * 40.0 + angle * 40.0 + u_time * timeMultiplier) * portalColor[1];
            color.b += sin(len * 40.0 + angle * 40.0 + u_time * timeMultiplier) * portalColor[2];
            color += glow * vec3(glowColor[0], glowColor[1] , glowColor[2]);
            break;

        case 1:
            // Generate swirl with two colors and glow
            color.r += sin(len * 40.0 + angle * 40.0 + u_time * timeMultiplier);
            color.g += cos(len * 40.0 + angle * 40.0 + u_time * timeMultiplier);
            color.b += sin(len * 40.0 + angle * 40.0 + u_time * timeMultiplier);
            color += glow * vec3(glowColor[0], glowColor[1] , glowColor[2]);
            break;
        case 2:
            // Generate swirl with two colors misaligned
            color.r += sin(len * 40.0 + angle * 40.0 + u_time * timeMultiplier);
            color.g += sin(len * 40.0 + angle * 40.0 - u_time * timeMultiplier);
            color.b += sin(len * 40.0 + angle * 40.0 + u_time * timeMultiplier);
            color += glow * vec3(glowColor[0], glowColor[1] , glowColor[2]);
            break;
        case 3:
            // Generate swirl with all colors misaligned
            color.r += sin(len * 40.0 + angle * 40.0 + u_time * timeMultiplier);
            color.g += cos(len * 30.0 + angle * 60.0 - u_time * timeMultiplier);
            color.b += sin(len * 50.0 + angle * 50.0 + 3.0);
            color += glow * vec3(glowColor[0], glowColor[1] , glowColor[2]);
            break;

    }

    // Apply the circular pattern with added distortion
    float radius = 0.5;
    float distortionStrength = 0.01;
    color *= CreateCircle(coord + vec2(0.0, 0.25), radius, fracture);

    // Set the final fragment color
    gl_FragColor = vec4(color, 1.0);
}
