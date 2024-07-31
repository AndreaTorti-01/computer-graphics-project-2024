#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec3 fragPos;
layout(location = 1) in vec3 fragNorm;
layout(location = 2) in vec2 fragTexCoord;

layout(location = 0) out vec4 outColor;

layout(binding = 1) uniform sampler2D texSampler;

layout(binding = 2) uniform GlobalUniformBufferObject {
	vec3 lightDir;
	vec4 lightColor;
	vec3 eyePos;
} gubo;

vec3 BRDF(vec3 V, vec3 N, vec3 L, vec3 Md, vec3 Ms) {

	vec3 Diffuse = Md * clamp(dot(N, L),0.0,1.0);

  float index = clamp(dot(N,L),0.0,1.0);

  if (index <= 0.0) Diffuse = vec3(0.0);
  else if (index > 0   && index <= 0.1) Diffuse = vec3(0.05);
  else if (index > 0.1 && index <= 0.7) Diffuse = vec3(0.1);
  else if (index > 0.7 && index <= 0.8) Diffuse = vec3(0.2);
  else Diffuse = vec3(0.35);

	return (Diffuse);
}

void main() {
	vec3 Norm = normalize(fragNorm);
	vec3 EyeDir = normalize(gubo.eyePos - fragPos);
	vec3 Albedo = texture(texSampler, fragTexCoord).rgb;
	vec3 L = gubo.lightDir;
	vec3 lightColor = gubo.lightColor.rgb;

	vec3 DiffSpec = BRDF(EyeDir, Norm, L, Albedo, vec3(1.0f));

	outColor = vec4(DiffSpec * (lightColor[0] * 0.3 + lightColor[1] * 0.8 + lightColor[2] * 0.5) * vec3(1.0), 1.0f);
}
