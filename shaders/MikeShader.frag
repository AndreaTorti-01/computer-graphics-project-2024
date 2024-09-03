#version 450
#extension GL_ARB_separate_shader_objects : enable

// this defines the variable received from the Vertex Shader
// the locations must match the one of its out variables
layout(location = 0) in vec3 fragPos;
layout(location = 1) in vec3 fragNorm;
layout(location = 2) in vec2 fragUV;
layout(location = 3) in flat int fragDamage;

// This defines the color computed by this shader. Generally is always location
// 0.
layout(location = 0) out vec4 outColor;

// Here the Uniform buffers are defined. In this case, the Global Uniforms of
// Set 0 The texture of Set 1 (binding 1), and the Material parameters (Set 1,
// binding 2) are used. Note that each definition must match the one used in the
// CPP code
layout(set = 0, binding = 0) uniform GlobalUniformBufferObject {
  vec3 lightDir[16];
  vec3 lightPos[16];
  vec4 lightColor[16];
  vec3 eyePos;
  float type[16];
}
gubo;

layout(set = 1, binding = 1) uniform sampler2D texSampler;


vec3 point_light_dir(vec3 pos, int i) {
  return normalize(gubo.lightPos[i] - pos);
}

vec3 point_light_color(vec3 pos, int i) {
  return gubo.lightColor[i].rgb *
         pow(gubo.lightColor[i].a / length(gubo.lightPos[i] - pos), 3.0f);
}

vec3 BRDF(vec3 V, vec3 N, vec3 L, vec3 Md) {

  vec3 Diffuse = Md * clamp(dot(N, L), 0.0, 1.0);

  float index = clamp(dot(N, L), 0.0, 1.0);

  if (index <= 0.0)
    Diffuse = vec3(0);
  else if (index > 0 && index <= 0.1)
    Diffuse = Md * index * 1.5;
  else if (index > 0.1 && index <= 0.7)
    Diffuse = Md * 0.15;
  else if (index > 0.7 && index <= 0.8)
    Diffuse = Md * (((index - 0.7) * 8.5) + 0.15);
  else
    Diffuse = Md;

  
	vec3 Specular = vec3(pow(clamp(dot(V, -reflect(L, N)),0.0,1.0), 200.0f)); 
  vec3 Ms = vec3(1.0);
  index = clamp(dot(V, -reflect(L, N)),0.0,1.0);

  if (index <= 0.95) Specular = vec3(0.0);
  else if (index > 0.95) Specular = Ms * ((index - 0.95) * 20);
  else Specular = Ms;

  return (Diffuse + Specular);
}

// The main shader, implementing a simple Globconst int NMIKE=15;al + Lambert + constant Ambient
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

  vec3 col = vec3(0.0);

  for (int i = 0; i < 16; i++) {
    if (i != 0)
      col += BRDF(EyeDir, Norm, point_light_dir(fragPos, i), Albedo) *
             point_light_color(fragPos, i);
    else
      col +=
          BRDF(EyeDir, Norm, gubo.lightDir[i], Albedo) * gubo.lightColor[i].rgb;
  }

  outColor = vec4(col + Ambient + fragDamage * vec3(0.8, 0.0, 0.0), 1.0f);
}