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

layout(set = 0, binding = 1) uniform sampler2D texSampler;


// The main shader, implementing a simple Global + Lambert + constant Ambient
// BRDF model The scene is lit by a single Spot Light
void main() {
  vec3 Norm = normalize(fragNorm);
  vec3 Albedo = texture(texSampler, fragUV).rgb;
  outColor = vec4(Albedo, 1.0);
}