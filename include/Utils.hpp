#pragma once

#include <vector>
#include <TextMaker.hpp>
#include <glm_with_defines.hpp>

constexpr auto MAX_MIKE_INSTANCES = 15;
constexpr auto MAX_BULLET_INSTANCES = 3;
constexpr auto MAX_UPGRADE_INSTANCES = 3;
constexpr auto FLOOR_DIAM = 128.0f; // multiple of 2
constexpr auto NLIGHTS = MAX_MIKE_INSTANCES + 1;
constexpr auto MAX_SPEED = 10.0f;

// Uniform buffer object for toon shading
struct ToonUniformBufferObject
{
	alignas(16) glm::mat4 mvpMat; // Model-View-Projection matrix
	alignas(16) glm::mat4 mMat;	  // Model matrix
	alignas(16) glm::mat4 nMat;	  // Normal matrix
};

struct MikeUniformBufferObject
{
	alignas(16) glm::mat4 mvpMat[MAX_MIKE_INSTANCES];
	alignas(16) glm::mat4 mMat[MAX_MIKE_INSTANCES];
	alignas(16) glm::mat4 nMat[MAX_MIKE_INSTANCES];
	alignas(16) float showDamage[MAX_MIKE_INSTANCES];
};

struct ToonParUniformBufferObject
{
	alignas(4) float textureMultiplier;
};

struct TrophyUniformBufferObject
{
	alignas(16) glm::mat4 mvpMat;
	alignas(16) glm::mat4 mMat; // Model matrix
	alignas(16) glm::mat4 nMat; // Normal matrix
	alignas(4) float prize;
};

// Uniform buffer object for skybox
struct SkyBoxUniformBufferObject
{
	alignas(16) glm::mat4 mvpMat; // Model-View-Projection matrix for skybox
};

struct GlobalUniformBufferObject
{
	struct
	{
		alignas(16) glm::vec3 v;
	} lightDir[NLIGHTS];
	struct
	{
		alignas(16) glm::vec3 v;
	} lightPos[NLIGHTS];
	struct
	{
		alignas(16) glm::vec4 v;
	} lightColor[NLIGHTS];

	alignas(16) glm::vec3 eyePos;

	struct
	{
		alignas(16) float t;
	} type[NLIGHTS]; // 0 global, 1 point
};

// Vertex structure for generic objects
struct GenericVertex
{
	glm::vec3 pos;	// Position
	glm::vec3 norm; // Normal
	glm::vec2 UV;	// Texture coordinates
};

// Vertex structure for skybox
struct SkyBoxVertex
{
	glm::vec3 pos; // Position (only position is needed for skybox)
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

struct UpgradeInstance
{
	glm::mat4 position4; // Position of Upgrade instance
	bool isAboveFloor;	 // Flag to indicate if Upgrade is above or below the floor
};