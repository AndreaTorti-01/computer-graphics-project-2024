#pragma once

#include <vector>
#include <TextMaker.hpp>
#include <glm_with_defines.hpp>

constexpr auto MAX_MIKE_INSTANCES = 15;
constexpr auto MAX_BULLET_INSTANCES = 3;
constexpr auto FLOOR_DIAM = 32.0f; // multiple of 2
constexpr auto NLIGHTS = 2;
constexpr auto MAX_SPEED = 10.0f;

// Uniform buffer object for toon shading
struct ToonUniformBufferObject {
	alignas(16) glm::mat4 mvpMat;  // Model-View-Projection matrix
	alignas(16) glm::mat4 mMat;    // Model matrix
	alignas(16) glm::mat4 nMat;    // Normal matrix
};

// Parameters for improved visuals of Mikes
struct MikeParUniformBufferObject {
	alignas(4) float showDamage;
};

// Uniform buffer object for skybox
struct SkyBoxUniformBufferObject {
	alignas(16) glm::mat4 mvpMat;  // Model-View-Projection matrix for skybox
};

struct GlobalUniformBufferObject
{
	alignas(16) glm::vec3 lightDir[NLIGHTS];  // in case of a point light, lightDir isn't it's direction, but the position
	alignas(16) glm::vec4 lightColor[NLIGHTS];
	alignas(16) glm::vec3 eyePos;
	alignas(4)  int type[NLIGHTS]; // 0 global, 1 point
};

// Vertex structure for generic objects
struct GenericVertex {
	glm::vec3 pos;   // Position
	glm::vec3 norm;  // Normal
	glm::vec2 UV;    // Texture coordinates
};

// Vertex structure for skybox
struct SkyBoxVertex {
	glm::vec3 pos;   // Position (only position is needed for skybox)
};

// Structure to represent an instance of Mike
struct MikeInstance
{
	glm::vec3 position; // Position of Mike instance
	float rotation;		// Rotation of Mike instance
	float damageTimer;
	bool isAboveFloor; // Flag to indicate if Mike is above or below the floor
	bool isDamaged;
};