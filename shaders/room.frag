#version 330 core
out vec4 FragColor;

in vec2 TexCoord;
in vec3 TangentLightPos;
in vec3 TangentViewPos;
in vec3 TangentFragPos;

uniform sampler2D uAlbedoMap;
uniform sampler2D uNormalMap;

uniform vec3 lightColor;

void main(){
    vec3 albedo = texture(uAlbedoMap, TexCoord).rgb;
    vec3 normalColor = texture(uNormalMap, TexCoord).rgb;
    vec3 tangentNormal = normalColor * 2.0 - 1.0;

    vec3 N = normalize(tangentNormal);
    vec3 lightDir = normalize(TangentLightPos - TangentFragPos);
    vec3 viewDir = normalize(TangentViewPos - TangentFragPos);

    float dist = length(TangentLightPos - TangentFragPos);
    // Adjust these to control how far the light reaches:
    float attenuation = 1.0 / (1.0 + 0.09 * dist + 0.032 * (dist * dist));

    // Minimal or no ambient:
    float ambientStrength = 0.0;
    vec3 ambient = ambientStrength * albedo;

    float diff = max(dot(N, lightDir), 0.0);
    vec3 diffuse = diff * albedo;

    float specularStrength = 0.5;
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(N, halfwayDir), 0.0), 16);
    vec3 specular = specularStrength * spec * vec3(1.0);

    vec3 lighting = (ambient + diffuse + specular) * lightColor * attenuation;
    FragColor = vec4(lighting,1.0);
}
