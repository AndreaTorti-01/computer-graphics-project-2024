#version 450
#extension GL_ARB_separate_shader_objects : enable

// this defines the variable received from the Vertex Shader
// the locations must match the one of its out variables
layout(location = 0) in vec3 fragPos;
layout(location = 1) in vec3 fragNorm;
layout(location = 2) in vec2 fragUV;

// This defines the color computed by this shader. Generally is always location
// 0.
layout(location = 0) out vec4 outColor;

// Here the Uniform buffers are defined. In this case, the Global Uniforms of
// Set 0 The texture of Set 1 (binding 1), and the Material parameters (Set 1,
// binding 2) are used. Note that each definition must match the one used in the
// CPP code
layout(set = 0, binding = 0) uniform BlinnUniformBufferObject {
  vec3 lightDir[2];
  vec4 lightColor[2];
  vec3 eyePos;
  int type[2];
}
gubo;

layout(set = 1, binding = 1) uniform sampler2D texSampler;
layout(set = 1, binding = 2) uniform MikeParUniformBufferObject {
	float showDamage;
}
pubo;

vec3 point_light_dir(vec3 pos, int i) {
  // Point light - direction vector
  // Position of the light in <gubo.lightPos[i]>
  return normalize(gubo.lightDir[i] - pos);
}

vec3 point_light_color(vec3 pos, int i) {
  // Point light - color
  // Color of the light in <gubo.lightColor[i].rgb>
  // Scaling factor g in <gubo.lightColor[i].a>
  // Decay power beta: constant and fixed to 2.0
  // Position of the light in <gubo.lightPos[i]>
  return gubo.lightColor[i].rgb *
         pow(gubo.lightColor[i].a / length(gubo.lightDir[i] - pos), 1.0);
}

vec3 BRDF(vec3 V, vec3 N, vec3 L, vec3 Md) {

  vec3 Diffuse = Md * clamp(dot(N, L), 0.0, 1.0);

  float index = clamp(dot(N, L), 0.0, 1.0);

  if (index <= 0.0) Diffuse = vec3(0.0);
  else if (index > 0   && index <= 0.1) Diffuse = Md * index * 1.5;
  else if (index > 0.1 && index <= 0.7) Diffuse = Md * 0.15;
  else if (index > 0.7 && index <= 0.8) Diffuse = Md * (((index - 0.7) * 8.5) + 0.15);
  else Diffuse = Md;

  return (Diffuse);
}

// The main shader, implementing a simple Blinn + Lambert + constant Ambient
// BRDF model The scene is lit by a single Spot Light
void main() {
  vec3 Norm = normalize(fragNorm);
  vec3 EyeDir = normalize(gubo.eyePos - fragPos);
  vec3 Albedo = texture(texSampler, fragUV).rgb;


  const vec3 cxp = vec3(1.0, 0.5, 0.5) * 0.15;
  const vec3 cxn = vec3(0.9, 0.6, 0.4) * 0.15;
  const vec3 cyp = vec3(0.3, 1.0, 1.0) * 0.15;
  const vec3 cyn = vec3(0.5, 0.5, 0.5) * 0.15;
  const vec3 czp = vec3(0.8, 0.2, 0.4) * 0.15;
  const vec3 czn = vec3(0.3, 0.6, 0.7) * 0.15;

  vec3 Ambient = ((Norm.x > 0 ? cxp : cxn) * (Norm.x * Norm.x) +
                  (Norm.y > 0 ? cyp : cyn) * (Norm.y * Norm.y) +
                  (Norm.z > 0 ? czp : czn) * (Norm.z * Norm.z)) *
                 Albedo;

   vec3 col = vec3(0.0f);

   for(int i=0; i<2; i++){
	if(gubo.type[i] == 0) col += BRDF(EyeDir, Norm, gubo.lightDir[i], Albedo) * gubo.lightColor[i].rgb;
	else col += BRDF(EyeDir, Norm, point_light_dir(fragPos, i), Albedo) * point_light_color(fragPos,i);
   }

   

  outColor = vec4(col + Ambient + vec3(1.0f,0.0f,0.0f) * pubo.showDamage, 1.0f);
}