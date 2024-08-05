// This has been adapted from the Vulkan tutorial

#include "modules/Starter.hpp"
#include "modules/TextMaker.hpp"
#include "modules/CarPosition.hpp"
#include <random>
#include <ctime>
#include <algorithm>


// Text to be displayed on screen
std::vector<SingleText> outText = {
	{2, {" ", " ", "", ""}, 0, 0},
	{1, {" ", "", "", ""}, 0, 0}
};

// Uniform buffer object for toon shading
struct ToonUniformBufferObject {
	alignas(16) glm::mat4 mvpMat;  // Model-View-Projection matrix
	alignas(16) glm::mat4 mMat;    // Model matrix
	alignas(16) glm::mat4 nMat;    // Normal matrix
};

// Uniform buffer object for skybox
struct SkyBoxUniformBufferObject {
	alignas(16) glm::mat4 mvpMat;  // Model-View-Projection matrix for skybox
};

#define NLIGHTS 2
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
struct MikeInstance {
	glm::vec3 position; // Position of Mike instance
	float rotation;     // Rotation of Mike instance
	bool isAboveFloor;  // Flag to indicate if Mike is above or below the floor
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
	Model MCar, MMike, MSkyBox, MFloor;
	Texture TGeneric, TMike, TSkyBox, TFloor, TCar;

	// Descriptor Sets
	DescriptorSet DSGlobal, DSCar, DSSkyBox, DSFloor;

	// Application parameters
	int currScene = 0;
	int subpass = 0;

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
	std::vector<DescriptorSet> DSMikes;
	float mikeSpawnTimer = 0.0f;
	std::mt19937 rng;  // Random number generator
	std::uniform_real_distribution<float> uniformDist;

	// Constants
	static const int MAX_MIKE_INSTANCES = 15;

	// Window parameters setup
	void setWindowParameters() override {
		windowWidth = 800;
		windowHeight = 600;
		windowTitle = "CG Project";
		windowResizable = GLFW_TRUE;
		initialBackgroundColor = { 0.1f, 0.1f, 0.1f, 1.0f };
		Ar = static_cast<float>(windowWidth) / static_cast<float>(windowHeight);
	}

	// Window resize handler
	void onWindowResize(int w, int h) override {
		std::cout << "Window resized to: " << w << " x " << h << "\n";
		Ar = static_cast<float>(w) / static_cast<float>(h);
	}

	// Initialization of local resources
	void localInit() {
		Ar = static_cast<float>(windowWidth) / static_cast<float>(windowHeight);

		// Initialize Descriptor Set Layouts
		DSLGlobal.init(this, { {0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_ALL_GRAPHICS, sizeof(GlobalUniformBufferObject), 1} });
		DSLToon.init(this, {
			{0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_ALL_GRAPHICS, sizeof(ToonUniformBufferObject), 1},
			{1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT, 0, 1},
			{2, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_ALL_GRAPHICS, sizeof(GlobalUniformBufferObject), 1}
			});
		DSLBW.init(this, {
			{0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_ALL_GRAPHICS, sizeof(ToonUniformBufferObject), 1},
			{1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT, 0, 1},
			{2, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_ALL_GRAPHICS, sizeof(GlobalUniformBufferObject), 1}
			});
		DSLSkyBox.init(this, {
			{0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT, sizeof(SkyBoxUniformBufferObject), 1},
			{1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT, 0, 1}
			});

		// Initialize Vertex Descriptors
		VDGeneric.init(this, { {0, sizeof(GenericVertex), VK_VERTEX_INPUT_RATE_VERTEX} }, {
			{0, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(GenericVertex, pos), sizeof(glm::vec3), POSITION},
			{0, 1, VK_FORMAT_R32G32B32_SFLOAT, offsetof(GenericVertex, norm), sizeof(glm::vec3), NORMAL},
			{0, 2, VK_FORMAT_R32G32_SFLOAT, offsetof(GenericVertex, UV), sizeof(glm::vec2), UV}
			});
		VDSkyBox.init(this, { {0, sizeof(SkyBoxVertex), VK_VERTEX_INPUT_RATE_VERTEX} }, {
			{0, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(SkyBoxVertex, pos), sizeof(glm::vec3), POSITION}
			});

		// Pipelines [Shader couples]
		PToon.init(this, &VDGeneric, "shaders/Vert.spv", "shaders/ToonFrag.spv", { &DSLGlobal, &DSLToon });
		PBW.init(this, &VDGeneric, "shaders/Vert.spv", "shaders/BWFrag.spv", { &DSLGlobal, &DSLBW });
		PSkyBox.init(this, &VDSkyBox, "shaders/SkyBoxVert.spv", "shaders/SkyBoxFrag.spv", { &DSLSkyBox });
		PSkyBox.setAdvancedFeatures(VK_COMPARE_OP_LESS_OR_EQUAL, VK_POLYGON_MODE_FILL, VK_CULL_MODE_BACK_BIT, false);

		// Create Models
		MCar.init(this, &VDGeneric, "models/Car.gltf", GLTF);
		MMike.init(this, &VDGeneric, "models/Mike.obj", OBJ);
		MSkyBox.init(this, &VDSkyBox, "models/SkyBox.obj", OBJ);
		MFloor.init(this, &VDGeneric, "models/Floor.obj", OBJ);

		// Initialize Textures
		TGeneric.init(this, "textures/Textures.png");
		TMike.init(this, "textures/T_Zebra.png");
		TSkyBox.init(this, "textures/T_SkyBox.jpg");
		TFloor.init(this, "textures/T_Floor.jpg");
		TCar.init(this, "textures/T_Car.jpg");

		// Set Descriptor Pool Sizes
		DPSZs.uniformBlocksInPool = 10 + MAX_MIKE_INSTANCES;
		DPSZs.texturesInPool = 10 + MAX_MIKE_INSTANCES;
		DPSZs.setsInPool = 10 + MAX_MIKE_INSTANCES;

		// Initialize text
		txt.init(this, &outText);

		// Initialize matrices
		ViewMatrix = glm::translate(glm::mat4(1), -CamPos);
		CarPos = glm::rotate(glm::rotate(glm::mat4(1), glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f)),
			glm::radians(180.0f), glm::vec3(0.0f, 0.0f, 1.0f));

		// Initialize random number generator
		rng.seed(static_cast<unsigned int>(std::time(nullptr)));
		uniformDist = std::uniform_real_distribution<float>(-10.0f, 10.0f);

		// Initialize Mike instances
		for (int i = 0; i < MAX_MIKE_INSTANCES; ++i) {
			mikes.push_back({ glm::vec3(uniformDist(rng), -10.0f, uniformDist(rng)), 0.0f, false });
		}
	}

	// Initialize pipelines and descriptor sets
	void pipelinesAndDescriptorSetsInit() {
		// Create pipelines
		PBW.create();
		PToon.create();
		PSkyBox.create();

		// Initialize descriptor sets
		DSCar.init(this, &DSLToon, { &TCar });
		DSMikes.resize(MAX_MIKE_INSTANCES);
		for (int i = 0; i < MAX_MIKE_INSTANCES; ++i) {
			DSMikes[i].init(this, &DSLBW, { &TMike });
		}
		DSFloor.init(this, &DSLToon, { &TFloor });
		DSSkyBox.init(this, &DSLSkyBox, { &TSkyBox });
		DSGlobal.init(this, &DSLGlobal, {});

		// Initialize text pipelines and descriptor sets
		txt.pipelinesAndDescriptorSetsInit();
	}

	// Cleanup pipelines and descriptor sets
	void pipelinesAndDescriptorSetsCleanup() {
		PToon.cleanup();
		PSkyBox.cleanup();
		PBW.cleanup();

		DSCar.cleanup();
		for (auto& ds : DSMikes) {
			ds.cleanup();
		}
		DSFloor.cleanup();
		DSSkyBox.cleanup();
		DSGlobal.cleanup();

		txt.pipelinesAndDescriptorSetsCleanup();
	}

	// Cleanup local resources
	void localCleanup() {
		TGeneric.cleanup();
		TMike.cleanup();
		TFloor.cleanup();
		TSkyBox.cleanup();
		TCar.cleanup();

		MCar.cleanup();
		MMike.cleanup();
		MSkyBox.cleanup();
		MFloor.cleanup();

		DSLToon.cleanup();
		DSLBW.cleanup();
		DSLSkyBox.cleanup();
		DSLGlobal.cleanup();

		PToon.destroy();
		PBW.destroy();
		PSkyBox.destroy();

		txt.localCleanup();
	}

	// Populate command buffer for rendering
	void populateCommandBuffer(VkCommandBuffer commandBuffer, int currentImage) {
		// Render Floor
		PToon.bind(commandBuffer);
		MFloor.bind(commandBuffer);
		DSGlobal.bind(commandBuffer, PToon, 0, currentImage);
		DSFloor.bind(commandBuffer, PToon, 1, currentImage);
		vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(MFloor.indices.size()), 1, 0, 0, 0);

		// Render Car
		PToon.bind(commandBuffer);
		MCar.bind(commandBuffer);
		DSGlobal.bind(commandBuffer, PToon, 0, currentImage);
		DSCar.bind(commandBuffer, PToon, 1, currentImage);
		vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(MCar.indices.size()), 1, 0, 0, 0);

		// Render Mike instances
		PBW.bind(commandBuffer);
		MMike.bind(commandBuffer);
		DSGlobal.bind(commandBuffer, PBW, 0, currentImage);
		for (size_t i = 0; i < mikes.size(); ++i) {
			DSMikes[i].bind(commandBuffer, PBW, 1, currentImage);
			vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(MMike.indices.size()), 1, 0, 0, 0);

		}

		// Render SkyBox
		PSkyBox.bind(commandBuffer);
		MSkyBox.bind(commandBuffer);
		DSSkyBox.bind(commandBuffer, PSkyBox, 0, currentImage);
		vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(MSkyBox.indices.size()), 1, 0, 0, 0);

		// Render text
		txt.populateCommandBuffer(commandBuffer, currentImage, currScene);
	}

	// Update uniform buffers
	void updateUniformBuffer(uint32_t currentImage) {

		static auto startTime = std::chrono::high_resolution_clock::now();
		auto currentTime = std::chrono::high_resolution_clock::now();
		float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();
		float deltaT;
		glm::vec3 m = glm::vec3(0.0f), r = glm::vec3(0.0f);
		bool fire = false;

		if (glfwGetKey(window, GLFW_KEY_C)) {
			glfwSetWindowShouldClose(window, 1);
			return;
		}
		getSixAxis(deltaT, m, r, fire);

		static glm::vec3 carPosition = glm::vec3(0.0f);
    	static float carRotation = 0.0f;
    	static float carSpeed = 0.0f;
    	static float currentSteeringAngle = 0.0f;
		const float MAX_SPEED = 8.0f;

		update_car_position(CarPos, carPosition, carSpeed, currentSteeringAngle, carRotation, m, deltaT);

		static glm::vec3 carPosition = glm::vec3(0.0f);
    	static float carRotation = 0.0f;
    	static float carSpeed = 0.0f;
    	static float currentSteeringAngle = 0.0f;
		const float MAX_SPEED = 8.0f;

		update_car_position(CarPos, carPosition, carSpeed, currentSteeringAngle, carRotation, m, deltaT);

		// Time-related variables for light movement
		static float autoTime = true;
		static float cTime = 0.0;
		const float turnTime = 72.0f;
		const float angTurnTimeFact = 2.0f * M_PI / turnTime;
		if (autoTime) {
			cTime = std::fmod(cTime + deltaT, turnTime);
		}

        // Update Mike instances
		mikeSpawnTimer += deltaT;
		if (mikeSpawnTimer >= 1.0f) {
			mikeSpawnTimer -= 1.0f;
			for (auto& mike : mikes) {
				if (!mike.isAboveFloor) {
					mike.position.y = 0.0f;
					mike.isAboveFloor = true;
					break;
				}
			}
		}

		for (auto& mike : mikes) {
			if (mike.isAboveFloor) {
				glm::vec3 dirToPlayer = glm::normalize(carPosition - mike.position);
				mike.position += dirToPlayer * 2.0f * deltaT;
				mike.rotation = std::atan2(dirToPlayer.x, dirToPlayer.z);

				float distanceToPlayer = glm::length(carPosition - mike.position);
				if (distanceToPlayer < 1.5f) {
					mike.position.y = -10.0f;
					mike.isAboveFloor = false;
				}
			}
		}

		// Camera position (static relative to the car)
		glm::vec3 cameraOffset = glm::vec3(5.0f, 15.0f, 5.0f);
		glm::vec3 cameraPosition = carPosition + cameraOffset;

		// Look at the car
		ViewMatrix = glm::lookAt(cameraPosition, carPosition, glm::vec3(0.0f, 1.0f, 0.0f));

		// Projection matrix with FOV adjustment based on speed
		float fov = glm::radians(30.0f + std::abs(carSpeed) * 0.75f);
		glm::mat4 M = glm::perspective(fov, Ar, 0.1f, 160.0f);
		M[1][1] *= -1; // Flip Y-axis for Vulkan coordinate system
		glm::mat4 Mv = ViewMatrix;
		glm::mat4 ViewPrj = M * Mv;

		// Shake camera slightly at high speed
		if (std::abs(carSpeed) > 0.9f * MAX_SPEED) {
			float shakeAmount = 0.004f * (std::abs(carSpeed) - 0.9f * MAX_SPEED) / (0.1f * MAX_SPEED);
			ViewPrj = glm::translate(ViewPrj, glm::vec3(
				0.0f,
				shakeAmount * (std::sin(time * 50.0f) + std::cos(time * 47.0f)),
				shakeAmount * (std::cos(time * 53.0f) + std::sin(time * 59.0f))
			));
		}

		// Update global uniforms
		GlobalUniformBufferObject uboGlobal{};
		uboGlobal.eyePos = glm::vec3(glm::inverse(ViewMatrix) * glm::vec4(0, 0, 0, 1));

		uboGlobal.lightDir[0] = glm::vec3 (0.0f, -1.0f, 0.0f);
		uboGlobal.lightColor[0] = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
		uboGlobal.type[0] = 0;

		uboGlobal.lightDir[1] = glm::vec3(5.0f,4.0f,5.0f);
		uboGlobal.lightColor[1] = glm::vec4(1.0f);
		uboGlobal.type[1] = 1;

		DSGlobal.map(currentImage, &uboGlobal, 0);

		// Update Car uniforms
		ToonUniformBufferObject uboCar{};
		uboCar.mMat = CarPos;
		uboCar.mvpMat = ViewPrj * uboCar.mMat;
		uboCar.nMat = glm::inverse(CarPos);
		DSCar.map(currentImage, &uboCar, 0);

		// Update Mike uniforms
		for (size_t i = 0; i < mikes.size(); ++i) {
			ToonUniformBufferObject uboMike{};
			glm::mat4 rotationMat = glm::rotate(glm::mat4(1.0f), mikes[i].rotation, glm::vec3(0.0f, 1.0f, 0.0f));
			uboMike.mMat = glm::translate(glm::mat4(1.0f), mikes[i].position) * rotationMat;
			uboMike.mvpMat = ViewPrj * uboMike.mMat;
			uboMike.nMat = glm::inverse(glm::transpose(uboMike.mMat));
			DSMikes[i].map(currentImage, &uboMike, 0);
		}

		// Update Floor uniforms
		ToonUniformBufferObject uboFloor{};
		uboFloor.mMat = glm::mat4(1.0f);
		uboFloor.mvpMat = ViewPrj * glm::mat4(1.0f);
		uboFloor.nMat = glm::transpose(glm::inverse(uboFloor.mMat));
		DSFloor.map(currentImage, &uboFloor, 0);

		// Update Skybox uniforms
		SkyBoxUniformBufferObject uboSky{};
		uboSky.mvpMat = M * glm::mat4(glm::mat3(Mv)); // Remove translation from view matrix
		DSSkyBox.map(currentImage, &uboSky, 0);
	}
};

// Main function
int main() {
	Application app;
	try {
		app.run();
	}
	catch (const std::exception& e) {
		std::cerr << e.what() << std::endl;
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}