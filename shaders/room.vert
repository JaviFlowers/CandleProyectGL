#version 330 core
layout(location=0) in vec3 aPos;
layout(location=1) in vec3 aNormal;
layout(location=2) in vec2 aTexCoord;
layout(location=3) in vec3 aTangent;
layout(location=4) in vec3 aBitangent;

uniform mat4 uProjection;
uniform mat4 uView;
uniform mat4 uModel;

out vec3 FragPos;
out vec2 TexCoord;
out vec3 TangentLightPos;
out vec3 TangentViewPos;
out vec3 TangentFragPos;
out mat3 TBN;

uniform vec3 lightPos;
uniform vec3 viewPos;

void main() {
    vec4 worldPos = uModel * vec4(aPos,1.0);
    FragPos = worldPos.xyz;
    TexCoord = aTexCoord;

    // Transform normals, tangents, bitangents
    vec3 T = normalize(mat3(uModel)*aTangent);
    vec3 B = normalize(mat3(uModel)*aBitangent);
    vec3 N = normalize(mat3(uModel)*aNormal);

    TBN = mat3(T, B, N);

    // Transform light and view position into tangent space
    vec3 lightDir = lightPos - FragPos;
    TangentLightPos = TBN * (lightPos - FragPos);
    TangentViewPos = TBN * (viewPos - FragPos);
    TangentFragPos = TBN * (FragPos - FragPos); // = vec3(0)

    gl_Position = uProjection * uView * worldPos;
}
