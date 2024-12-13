#version 330 core
layout(location=0) in vec3 aPos;

uniform mat4 uProjection;
uniform mat4 uView;

void main(){
    gl_Position = uProjection * uView * vec4(aPos,1.0);
    // Adjust point size if needed, or pass as uniform
    gl_PointSize = 15.0; // smaller for core flame emitter, larger for haze emitter
}
