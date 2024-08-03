#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec3 fragPos;
layout(location = 1) in vec3 fragNorm;
layout(location = 2) in vec2 fragUV;
layout(location = 3) in vec4 fragTangent;  // Tangent vector as vec4

layout(location = 0) out vec4 outColor;

layout(set = 0, binding = 0) uniform BlinnUniformBufferObject {
    vec3 lightDir;
    vec4 lightColor;
    vec3 eyePos;
} gubo;

layout(set = 1, binding = 1) uniform sampler2D texSampler;
layout(set = 1, binding = 2) uniform sampler2D normalMap;  // Normal map texture

vec3 ToonBRDF(vec3 V, vec3 N, vec3 L, vec3 Md, vec3 Ms) {
    float NdotL = dot(N, L);
    
    // Toon diffuse
    vec3 Diffuse;
    if (NdotL <= 0.0) Diffuse = vec3(0.0);
    else if (NdotL <= 0.1) Diffuse = Md * NdotL * 1.5;
    else if (NdotL <= 0.7) Diffuse = Md * 0.15;
    else if (NdotL <= 0.8) Diffuse = Md * (((NdotL - 0.7) * 8.5) + 0.15);
    else Diffuse = Md;

    // Toon specular
    float spec = dot(V, reflect(-L, N));
    vec3 Specular;
    if (spec <= 0.9) Specular = vec3(0.0);
    else if (spec <= 0.95) Specular = Ms * ((spec - 0.9) * 20.0);
    else Specular = Ms;

    return (Diffuse + Specular);
}

void main() {
    vec3 N = normalize(fragNorm);
    vec3 T = normalize(fragTangent.xyz);
    vec3 B = cross(N, T) * fragTangent.w;  // Bitangent
    mat3 TBN = mat3(T, B, N);

    // Sample normal from normal map
    vec3 normalMap = texture(normalMap, fragUV).rgb;
    normalMap = normalMap * 2.0 - 1.0;  // Convert from [0,1] to [-1,1]
    vec3 Norm = normalize(TBN * normalMap);  // Transform normal to world space

    vec3 EyeDir = normalize(gubo.eyePos - fragPos);
    vec3 Albedo = texture(texSampler, fragUV).rgb;
    vec3 lightDir = normalize(gubo.lightDir);
    vec3 lightColor = gubo.lightColor.rgb;

    // Stylized ambient lighting
    const vec3 cxp = vec3(1.0,0.5,0.5) * 0.15;
    const vec3 cxn = vec3(0.9,0.6,0.4) * 0.15;
    const vec3 cyp = vec3(0.3,1.0,1.0) * 0.15;
    const vec3 cyn = vec3(0.5,0.5,0.5) * 0.15;
    const vec3 czp = vec3(0.8,0.2,0.4) * 0.15;
    const vec3 czn = vec3(0.3,0.6,0.7) * 0.15;
    
    vec3 Ambient = ((Norm.x > 0 ? cxp : cxn) * (Norm.x * Norm.x) +
                    (Norm.y > 0 ? cyp : cyn) * (Norm.y * Norm.y) +
                    (Norm.z > 0 ? czp : czn) * (Norm.z * Norm.z)) * Albedo;

    vec3 DiffSpec = ToonBRDF(EyeDir, Norm, lightDir, Albedo, vec3(1.0));
    
    outColor = vec4(DiffSpec * lightColor + Ambient, 1.0);
}