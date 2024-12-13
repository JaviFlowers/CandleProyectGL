#version 330 core
out vec4 FragColor;

in vec3 vColor; // If you pass per-particle color or just do color calc here
uniform vec4 uColor; // Base color from CPU if desired

void main(){
    // Distance from center of the point sprite
    vec2 coord = gl_PointCoord - vec2(0.5);
    float dist = length(coord);

    // Create a gradient: 0 at center, 1 at edge
    // If the particle radius is assumed, say radius ~0.5 for full sprite
    // We'll fade out beyond dist ~0.5
    float radius = 0.5;
    float alpha = 1.0 - smoothstep(0.0, radius, dist);

    // Color gradient:
    // At center: nearly white/yellow (1.0, 0.9, 0.5)
    // At edges: more orange/blue mix (1.0,0.5,0.0) or (0.0,0.2,0.8)
    // We can blend between two colors based on dist
    vec3 centerColor = vec3(1.0, 0.9, 0.5);
    vec3 edgeColor = mix(vec3(1.0,0.5,0.0), vec3(0.0,0.2,0.8), dist*2.0); 
    // dist*2.0 shifts color between orange and bluish as you go outward

    vec3 finalColor = mix(centerColor, edgeColor, dist/radius);

    FragColor = vec4(finalColor, alpha);
}
