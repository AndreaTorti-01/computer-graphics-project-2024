#version 450
#extension GL_ARB_separate_shader_objects : enable

// this defines the variable received from the Vertex Shader
// the locations must match the one of its out variables
layout(location = 0) in vec3 fragPos;
layout(location = 1) in vec3 fragNorm;
layout(location = 2) in vec2 fragUV;

// This defines the color computed by this shader. Generally is always location 0.
layout(location = 0) out vec4 outColor;

// Here the Uniform buffers are defined. In this case, the Global Uniforms of Set 0
// The texture of Set 1 (binding 1), and the Material parameters (Set 1, binding 2)
// are used. Note that each definition must match the one used in the CPP code
layout(set = 0, binding = 0) uniform BlinnUniformBufferObject {
	vec3 lightDir;
	vec4 lightColor;
	vec3 eyePos;
} gubo;

layout(set = 1, binding = 2) uniform BlinnParUniformBufferObject {
	float Pow;
} mubo;

layout(set = 1, binding = 1) uniform sampler2D texSampler;


vec3 BRDF(vec3 V, vec3 N, vec3 L, vec3 Md, vec3 Ms) {

  vec3 Diffuse = Md * clamp(dot(N, L),0.0,1.0);

  float index = clamp(dot(N,L),0.0,1.0);

  if (index <= 0.0) Diffuse = vec3(0.0);
  else if (index > 0   && index <= 0.1) Diffuse = vec3(0.05);
  else if (index > 0.1 && index <= 0.7) Diffuse = vec3(0.4);
  else if (index > 0.7 && index <= 0.8) Diffuse = vec3(0.6);
  else Diffuse = vec3(0.8);

  return (Diffuse);
}

// The main shader, implementing a simple Blinn + Lambert + constant Ambient BRDF model
// The scene is lit by a single Spot Light
void main() {
	vec3 Norm = normalize(fragNorm);
	vec3 EyeDir = normalize(gubo.eyePos - fragPos);
	vec3 Albedo = texture(texSampler, fragUV).rgb;
	vec3 lightDir = gubo.lightDir;
	vec3 lightColor = gubo.lightColor.rgb;

	const vec3 cxp = vec3(1.0,0.5,0.5) * 0.15;
	const vec3 cxn = vec3(0.9,0.6,0.4) * 0.15;
	const vec3 cyp = vec3(0.3,1.0,1.0) * 0.15;
	const vec3 cyn = vec3(0.5,0.5,0.5) * 0.15;
	const vec3 czp = vec3(0.8,0.2,0.4) * 0.15;
	const vec3 czn = vec3(0.3,0.6,0.7) * 0.15;

	vec3 DiffSpec = BRDF(EyeDir, Norm, lightDir, Albedo, vec3(1.0f));
	
	outColor = vec4(DiffSpec * vec3(1.0f), 1.0f);
}