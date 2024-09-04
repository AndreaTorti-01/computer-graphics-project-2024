#version 450
#extension GL_ARB_separate_shader_objects : enable

// this defines the variable received from the Vertex Shader
// the locations must match the one of its out variables
layout(location = 0) in vec3 fragPos;
layout(location = 1) in vec3 fragNorm;
layout(location = 2) in vec2 fragUV;
layout(location = 3) in flat int prize;

// This defines the color computed by this shader. Generally is always location
// 0.
layout(location = 0) out vec4 outColor;

// Here the Uniform buffers are defined. In this case, the Global Uniforms of
// Set 0 The texture of Set 1 (binding 1), and the Material parameters (Set 1,
// binding 2) are used. Note that each definition must match the one used in the
// CPP code

layout(set = 0, binding = 1) uniform sampler2D texFirst;
layout(set = 0, binding = 2) uniform sampler2D texSecond;
layout(set = 0, binding = 3) uniform sampler2D texThird;

// The main shader, implementing a simple Global + Lambert + constant Ambient
// BRDF model The scene is lit by a single Spot Light
void main() {
  vec3 Norm = normalize(fragNorm);
  vec3 Albedo;

  
  switch (prize) {
  case 1:
    Albedo = texture(texFirst, fragUV).rgb;
    break;
  case 2:
    Albedo = texture(texSecond, fragUV).rgb;
    break;
  case 3:
    Albedo = texture(texThird, fragUV).rgb;
    break;
  }

  const vec3 cxp = vec3(0.7, 0.7, 0.7) * 0.5;
  const vec3 cxn = vec3(0.2, 0.2, 0.2) * 0.5;
  const vec3 cyp = vec3(0.7, 0.7, 0.7) * 0.5;
  const vec3 cyn = vec3(0.2, 0.2, 0.2) * 0.5;
  const vec3 czp = vec3(0.7, 0.7, 0.7) * 0.5;
  const vec3 czn = vec3(0.2, 0.2, 0.2) * 0.5;

  vec3 Ambient = ((Norm.x > 0 ? cxp : cxn) * (Norm.x * Norm.x) +
                  (Norm.y > 0 ? cyp : cyn) * (Norm.y * Norm.y) +
                  (Norm.z > 0 ? czp : czn) * (Norm.z * Norm.z)) *
                 Albedo;

  outColor = vec4(Albedo + Ambient, 1.0);
}