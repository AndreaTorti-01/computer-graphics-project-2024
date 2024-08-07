#pragma once
// This has been adapted from the Vulkan tutorial

#include "starter.hpp"
#include "TextMaker.hpp"

#include <vector>
#include <random>

constexpr auto MAX_MIKE_INSTANCES = 15;
constexpr auto MAX_BULLET_INSTANCES = 8;
constexpr auto FLOOR_DIAM = 32.0f; // multiple of 2
constexpr auto NLIGHTS = 2;

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

// Structure to represent an instance of a bullet
struct BulletInstance
{
	glm::vec3 position;
	glm::vec3 dir;
	float lifetime;
	bool isAboveFloor;
};

// Main application class
class Application : public BaseProject {
protected:
	// Descriptor Layouts
	DescriptorSetLayout DSLGlobal, DSLSkyBox, DSLToon, DSLBW;

	// Vertex formats
	VertexDescriptor VDGeneric, VDSkyBox;

	// Pipelines
	Pipeline PToon, PBW, PSkyBox;

	// Text maker
	TextMaker txt;

	// Models and textures
	Model MCar, MMike, MSkyBox, MFloor, MBullet;
	Texture TGeneric, TMike, TSkyBox, TFloor, TCar, TBullet;

	// Descriptor Sets
	DescriptorSet DSGlobal, DSCar, DSSkyBox, DSFloor;

	// Application parameters
	int currScene = 0;
	int subpass = 0;
	int score = 0;
	int health = 10;

	// Camera parameters
	glm::vec3 CamPos = glm::vec3(0.0, 1.5, 7.0);
	float CamAlpha = 0.0f;
	float CamBeta = 0.0f;
	float Ar;  // Aspect ratio
	glm::mat4 ViewMatrix;

	// Car position matrix
	glm::mat4 CarPos;

	// Mike instances
	std::vector<MikeInstance> mikes;
	std::vector<BulletInstance> bullets;
	std::vector<DescriptorSet> DSMikes;
	std::vector<DescriptorSet> DSBullets;
	float mikeSpawnTimer = 0.0f;
	float fireTimer = 0.0f;
	std::mt19937 rng;  // Random number generator
	std::uniform_real_distribution<float> uniformDist;

	// Descriptor pool sizes
	int totalUniformBlocks = 0;
	int totalTextures = 0;
	int totalSets = 0;

	// RNG
	std::random_device rd;

	// mike spawn radii
	float minRadius = 3.0f;
	float maxRadius = 7.0f;

	// Text to be displayed on screen
	std::vector<SingleText> outText = {
		{2, {" ", " ", "", ""}, 0, 0},
		{1, {" ", "", "", ""}, 0, 0}
	};

	// Window parameters setup
	void setWindowParameters() override;

	// Window resize handler
	void onWindowResize(int w, int h) override;


	// Initialization of local resources
	void localInit() override;

	// Initialize pipelines and descriptor sets
	void pipelinesAndDescriptorSetsInit() override;

	// Cleanup pipelines and descriptor sets
	void pipelinesAndDescriptorSetsCleanup() override;

	// Cleanup local resources
	void localCleanup() override;

	// Populate command buffer for rendering
	void populateCommandBuffer(VkCommandBuffer commandBuffer, int currentImage) override;

	// Update uniform buffers
	void updateUniformBuffer(uint32_t currentImage) override;

	void calculateDescriptorPoolSizes();

	void update_bullet_positions(glm::vec3 carPos, std::vector<BulletInstance>& bullets, glm::vec3 direction, float& fireTimer, float deltaT);

	void update_car_position(glm::mat4& CarPos, glm::vec3& carPosition, float& carSpeed, float& currentSteeringAngle, float& carRotation, glm::vec3& m, const float deltaT, const float floordiam);

	// Function to generate a random position around the car within the floor boundaries
	glm::vec3 generateRandomPosition(const glm::vec3& carPosition, const float minRadius, const float maxRadius, std::mt19937& rng, const float floorDiam);

	void update_mike_positions(const glm::vec3& carPosition, std::vector<MikeInstance>& mikes, float& mikeSpawnTimer, const float deltaT, std::mt19937& rng, const float minRadius, const float maxRadius, const float floorDiam);

	void check_collisions_MB(std::vector<MikeInstance>& mikes, std::vector<BulletInstance>& bullets);

	bool check_collisions_MC(std::vector<MikeInstance>& mikes, glm::vec3& CarPosition);
};
