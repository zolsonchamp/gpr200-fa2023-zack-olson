#version 330 core

// Uniform variables
uniform float u_time;
uniform float timeMultiplier;
uniform vec2 u_resolution;
uniform vec4 portalColor;
uniform int setColor;
uniform vec2 portalPos;

// Function to create a circular pattern using length
float circle_using_length(vec2 position, float radius) {
    // Calculate the displacement vector from the center
    vec2 d = position - vec2(0.5);
    
    // Correct for aspect ratio
    d.x *= u_resolution.x / u_resolution.y;
    
    // Use step function to create a circular shape
    return 1.0 - step(radius, length(d));
}

void main() {

    // Calculate normalized coordinates for the current fragment
    vec2 coord = (gl_FragCoord.xy / u_resolution);
    //vec2 coord = vec2(0.0f, -1.0f);
    
    // Shift the y-coordinate to create a vertical offset
    coord.y -= 0.25f;
    
    // Initialize color as black
    vec3 color = vec3(0.0);

    // Calculate angle based on the modified coordinates
    float angle = atan(-coord.y + 0.25, coord.x - 0.5) * 0.1;
    
    // Calculate the distance from the center
    float len = length(coord - vec2(0.5, 0.25));

    switch(setColor)
    {

        case 0:
            // Generate swirl with manual color
            color.r += sin(len * 40.0 + angle * 40.0 + u_time * timeMultiplier) * portalColor[0];
            color.g += sin(len * 40.0 + angle * 40.0 + u_time * timeMultiplier) * portalColor[1];
            color.b += sin(len * 40.0 + angle * 40.0 + u_time * timeMultiplier) * portalColor[2];
            break;

        case 1:
            // Generate swirl with two colors
            color.r += sin(len * 40.0 + angle * 40.0 + u_time * timeMultiplier);
            color.g += cos(len * 40.0 + angle * 40.0 + u_time * timeMultiplier);
            color.b += sin(len * 40.0 + angle * 40.0 + u_time * timeMultiplier);
            break;
        case 2:
            // Generate swirl with two colors misaligned
            color.r += sin(len * 40.0 + angle * 40.0 + u_time * timeMultiplier);
            color.g += sin(len * 40.0 + angle * 40.0 - u_time * timeMultiplier);
            color.b += sin(len * 40.0 + angle * 40.0 + u_time * timeMultiplier);
            break;
        case 3:
            // Generate swirl with all colors misaligned
            color.r += sin(len * 40.0 + angle * 40.0 + u_time * timeMultiplier);
            color.g += cos(len * 30.0 + angle * 60.0 - u_time * timeMultiplier);
            color.b += sin(len * 50.0 + angle * 50.0 + 3.0);
            break;
    }

    // Apply the circular pattern using the defined function
    color *= circle_using_length(coord + vec2(0.0, 0.22), 0.35);

    // Set the final fragment color
    gl_FragColor = vec4(color, 1.0);
}