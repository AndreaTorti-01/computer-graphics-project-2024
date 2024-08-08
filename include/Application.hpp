#pragma once
// This has been adapted from the Vulkan tutorial

#include "starter.hpp"
#include "TextMaker.hpp"
#include "Bullet.hpp"

#include <vector>
#include <random>

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
	std::vector<Bullet> bullets;
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

	void update_bullet_positions(glm::vec3 carPos, std::vector<Bullet>& bullets, glm::vec3 direction, float& fireTimer, float deltaT);

	void update_car_position(glm::mat4& CarPos, glm::vec3& carPosition, float& carSpeed, float& currentSteeringAngle, float& carRotation, glm::vec3& m, const float deltaT, const float floordiam);

	// Function to generate a random position around the car within the floor boundaries
	glm::vec3 generateRandomPosition(const glm::vec3& carPosition, const float minRadius, const float maxRadius, std::mt19937& rng, const float floorDiam);

	void update_mike_positions(const glm::vec3& carPosition, std::vector<MikeInstance>& mikes, float& mikeSpawnTimer, const float deltaT, std::mt19937& rng, const float minRadius, const float maxRadius, const float floorDiam);

	void check_collisions_MB(std::vector<MikeInstance>& mikes, std::vector<Bullet>& bullets);

	bool check_collisions_MC(std::vector<MikeInstance>& mikes, glm::vec3& CarPosition);
};
