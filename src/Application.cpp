#include "Application.hpp"

// Window parameters setup
void Application::setWindowParameters() {
	windowWidth = 800;
	windowHeight = 600;
	windowTitle = "CG Project";
	windowResizable = GLFW_TRUE;
	initialBackgroundColor = { 0.1f, 0.1f, 0.1f, 1.0f };
	Ar = static_cast<float>(windowWidth) / static_cast<float>(windowHeight);
}

// Window resize handler
void Application::onWindowResize(int w, int h) {
	std::cout << "Window resized to: " << w << " x " << h << "\n";
	Ar = static_cast<float>(w) / static_cast<float>(h);
}

// Add this new function to calculate descriptor pool sizes
void Application::calculateDescriptorPoolSizes() {
	// Global Uniform Buffer
	totalUniformBlocks += 1;
	totalSets += 1;

	// Car
	totalUniformBlocks += 1;
	totalTextures += 1;
	totalSets += 1;

	// Mike instances
	totalUniformBlocks += MAX_MIKE_INSTANCES;
	totalTextures += MAX_MIKE_INSTANCES;
	totalSets += MAX_MIKE_INSTANCES;

	// Bullet instances
	totalUniformBlocks += MAX_BULLET_INSTANCES;
	totalTextures += MAX_BULLET_INSTANCES;
	totalSets += MAX_BULLET_INSTANCES;

	// Floor
	totalUniformBlocks += 1;
	totalTextures += 1;
	totalSets += 1;

	// Skybox
	totalUniformBlocks += 1;
	totalTextures += 1;
	totalSets += 1;

	// Add a small buffer for safety
	totalUniformBlocks += 5;
	totalTextures += 5;
	totalSets += 5;

	// Set the calculated values
	DPSZs.uniformBlocksInPool = totalUniformBlocks;
	DPSZs.texturesInPool = totalTextures;
	DPSZs.setsInPool = totalSets;
}

// Initialization of local resources
void Application::localInit() {
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
		{2, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_FRAGMENT_BIT, sizeof(MikeParUniformBufferObject), 1},
		{3, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_ALL_GRAPHICS, sizeof(GlobalUniformBufferObject), 1}
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
	MFloor.init(this, &VDGeneric, "models/newFloor.obj", OBJ);
	MBullet.init(this, &VDGeneric, "models/Car.mgcg", MGCG);

	// Initialize Textures
	TGeneric.init(this, "textures/Textures.png");
	TMike.init(this, "textures/T_Zebra.png");
	TSkyBox.init(this, "textures/T_SkyBox.jpg");
	TFloor.init(this, "textures/TCom_Pavement_TerracottaAntique_2K_albedo.jpg");
	TCar.init(this, "textures/T_Car.jpg");
	TBullet.init(this, "textures/Textures.png");

	calculateDescriptorPoolSizes();

	// Initialize RNG
	std::mt19937 rng(rd());

	// Initialize text
	txt.init(this, &outText);

	// Initialize matrices
	ViewMatrix = glm::translate(glm::mat4(1), -CamPos);
	CarPos = glm::rotate(glm::rotate(glm::mat4(1), glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f)),
		glm::radians(180.0f), glm::vec3(0.0f, 0.0f, 1.0f));

	// Initialize Mike instances
	for (int i = 0; i < MAX_MIKE_INSTANCES; ++i) {
		mikes.push_back({ glm::vec3(0.0f, -10.0f, 0.0f), 0.0f, 0.0f, false, false });
	}

	for (int i = 0; i < MAX_BULLET_INSTANCES; ++i) {
		bullets.push_back(Bullet());
	}
}

// Initialize pipelines and descriptor sets
void Application::pipelinesAndDescriptorSetsInit() {

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
	DSBullets.resize(MAX_BULLET_INSTANCES);
	for (int i = 0; i < MAX_BULLET_INSTANCES; ++i) {
		DSBullets[i].init(this, &DSLToon, { &TBullet });
	}
	DSFloor.init(this, &DSLToon, { &TFloor });
	DSSkyBox.init(this, &DSLSkyBox, { &TSkyBox });
	DSGlobal.init(this, &DSLGlobal, {});

	// Initialize text pipelines and descriptor sets
	txt.pipelinesAndDescriptorSetsInit();
}

// Cleanup pipelines and descriptor sets
void Application::pipelinesAndDescriptorSetsCleanup() {
	PToon.cleanup();
	PSkyBox.cleanup();
	PBW.cleanup();

	DSCar.cleanup();
	for (auto& ds : DSMikes) {
		ds.cleanup();
	}
	for (auto& ds : DSBullets) {
		ds.cleanup();
	}
	DSFloor.cleanup();
	DSSkyBox.cleanup();
	DSGlobal.cleanup();

	txt.pipelinesAndDescriptorSetsCleanup();
}

// Cleanup local resources
void Application::localCleanup() {
	TGeneric.cleanup();
	TMike.cleanup();
	TFloor.cleanup();
	TSkyBox.cleanup();
	TCar.cleanup();
	TBullet.cleanup();

	MCar.cleanup();
	MMike.cleanup();
	MSkyBox.cleanup();
	MFloor.cleanup();
	MBullet.cleanup();

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
void Application::populateCommandBuffer(VkCommandBuffer commandBuffer, int currentImage) {
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

	// Render Bullet instances
	PToon.bind(commandBuffer);
	MBullet.bind(commandBuffer);
	DSGlobal.bind(commandBuffer, PToon, 0, currentImage);
	for (size_t i = 0; i < bullets.size(); ++i) {
		DSBullets[i].bind(commandBuffer, PToon, 1, currentImage);
		vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(MBullet.indices.size()), 1, 0, 0, 0);
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
void Application::updateUniformBuffer(uint32_t currentImage) {

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



	update_car_position(CarPos, carPosition, carSpeed, currentSteeringAngle, carRotation, m, deltaT, FLOOR_DIAM);
	update_mike_positions(carPosition, mikes, mikeSpawnTimer, deltaT, rng, minRadius, maxRadius, FLOOR_DIAM);
	for(int i = 0; i < bullets.size(); i++) bullets[i].update(deltaT);

	check_collisions_MB(mikes, bullets);
	if (check_collisions_MC(mikes, carPosition)) health--;

	//std::cout<<"S:"<<score<<"\n"<<"H:"<<health<<"\n";

	// Time-related variables for light movement
	static float autoTime = true;
	static float cTime = 0.0;
	const float turnTime = 72.0f;
	const float angTurnTimeFact = 2.0f * M_PI / turnTime;
	if (autoTime) {
		cTime = std::fmod(cTime + deltaT, turnTime);
	}

	// Camera position (static relative to the car)
	glm::vec3 cameraOffset = glm::vec3(5.0f, 20.0f, 5.0f);
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

	uboGlobal.lightDir[0] = glm::vec3(0.0f, -1.0f, 0.0f);
	uboGlobal.lightColor[0] = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	uboGlobal.type[0] = 0;

	uboGlobal.lightDir[1] = glm::vec3(5.0f, 4.0f, 5.0f);
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
		MikeParUniformBufferObject uboMikePar{};
		glm::mat4 rotationMat = glm::rotate(glm::mat4(1.0f), mikes[i].rotation, glm::vec3(0.0f, 1.0f, 0.0f));
		uboMike.mMat = glm::translate(glm::mat4(1.0f), mikes[i].position) * rotationMat;
		uboMike.mvpMat = ViewPrj * uboMike.mMat;
		uboMike.nMat = glm::inverse(glm::transpose(uboMike.mMat));
		uboMikePar.showDamage = mikes[i].isDamaged;
		DSMikes[i].map(currentImage, &uboMike, 0);
		DSMikes[i].map(currentImage, &uboMikePar, 2);
	}


	// Update Bullet uniforms
	for (size_t i = 0; i < bullets.size(); ++i) {
		if (bullets[i].getIsAboveFloor()) {
			ToonUniformBufferObject uboBullet{};
			uboBullet.mMat = glm::translate(glm::mat4(1.0f), bullets[i].getPosition());
			uboBullet.mvpMat = ViewPrj * uboBullet.mMat;
			uboBullet.nMat = glm::inverse(glm::transpose(uboBullet.mMat));
			DSBullets[i].map(currentImage, &uboBullet, 0);
		}
	}

	// Update Floor uniforms
	ToonUniformBufferObject uboFloor{};
	uboFloor.mMat = glm::scale(glm::mat4(1.0f), glm::vec3(FLOOR_DIAM));
	uboFloor.mvpMat = ViewPrj * uboFloor.mMat;
	uboFloor.nMat = glm::transpose(glm::inverse(uboFloor.mMat));

	DSFloor.map(currentImage, &uboFloor, 0);

	// Update Skybox uniforms
	SkyBoxUniformBufferObject uboSky{};
	uboSky.mvpMat = M * glm::mat4(glm::mat3(Mv)); // Remove translation from view matrix
	DSSkyBox.map(currentImage, &uboSky, 0);
}


void Application::update_car_position(glm::mat4& CarPos, glm::vec3& carPosition, float& carSpeed, float& currentSteeringAngle, float& carRotation, glm::vec3& m, const float deltaT, const float floordiam)
{
	// Car properties
	float MAX_SPEED = 8.0f;
	float ACCELERATION = 5.0f;
	float DECELERATION = 20.0f;
	float MAX_STEERING_ANGLE = glm::radians(35.0f);
	float STEERING_SPEED = glm::radians(180.0f);
	float WHEELBASE = 2.0f;

	// Update car speed
	if (m.z > 0.1f)
	{
		if (carSpeed < 0)
		{
			// If moving backward and forward button is pressed, decelerate using DECELERATION
			carSpeed = std::min(carSpeed + DECELERATION * deltaT, 0.0f);
		}
		else
		{
			carSpeed = std::min(carSpeed + ACCELERATION * deltaT, MAX_SPEED);
		}
	}
	else if (m.z < -0.1f)
	{
		if (carSpeed > 0)
		{
			// If moving forward and backward button is pressed, decelerate using DECELERATION
			carSpeed = std::max(carSpeed - DECELERATION * deltaT, 0.0f);
		}
		else
		{
			carSpeed = std::max(carSpeed - ACCELERATION * deltaT, -MAX_SPEED);
		}
	}
	else
	{
		// Apply natural deceleration when no input
		carSpeed = carSpeed > 0 ? std::max(carSpeed - ACCELERATION * deltaT, 0.0f) : carSpeed < 0 ? std::min(carSpeed + ACCELERATION * deltaT, 0.0f)
			: 0.0f;
	}

	// Update steering angle incrementally
	float targetSteeringAngle = -MAX_STEERING_ANGLE * m.x;
	float steeringDelta = STEERING_SPEED * deltaT;
	currentSteeringAngle = glm::clamp(
		currentSteeringAngle + glm::clamp(targetSteeringAngle - currentSteeringAngle, -steeringDelta, steeringDelta),
		-MAX_STEERING_ANGLE, MAX_STEERING_ANGLE);

	// Update car position and rotation using Ackermann steering
	if (std::abs(carSpeed) > 0.001f)
	{
		float turnRadius = WHEELBASE / std::tan(std::abs(currentSteeringAngle) + 1e-5);
		float angularVelocity = carSpeed / turnRadius;

		carRotation += angularVelocity * deltaT * glm::sign(currentSteeringAngle);
		carPosition += glm::vec3(
			std::cos(carRotation) * carSpeed * deltaT,
			0.0f,
			std::sin(carRotation) * carSpeed * deltaT);
	}

	// Constrain the car position to the floor size
	const float floorDiamAdj = (floordiam - 1.0f) / 2.0f;
	carPosition.x = glm::clamp(carPosition.x, -floorDiamAdj, floorDiamAdj);
	carPosition.z = glm::clamp(carPosition.z, -floorDiamAdj, floorDiamAdj);

	// Tilt the car based on amount of acceleration
	float carTilt = glm::radians(glm::clamp(m.z * -5.0f, -5.0f, 5.0f));

	// Update car matrix
	CarPos = glm::translate(glm::mat4(1.0f), carPosition);
	CarPos = glm::rotate(CarPos, -carRotation, glm::vec3(0.0f, 1.0f, 0.0f));
	CarPos = glm::rotate(CarPos, -carTilt, glm::vec3(0.0f, 0.0f, 1.0f));
	// rotate model by 90 degrees clockwise and scale it 3x
	CarPos = glm::scale(CarPos, glm::vec3(3.0f, 3.0f, 3.0f));
	CarPos = glm::rotate(CarPos, glm::radians(90.0f), glm::vec3(0.0f, -1.0f, 0.0f));
}

// Function to generate a random position around the car within the floor boundaries
glm::vec3 Application::generateRandomPosition(const glm::vec3& carPosition, const float minRadius, const float maxRadius, std::mt19937& rng, const float floorDiam) {
	std::uniform_real_distribution<float> distRadius(minRadius, maxRadius);
	std::uniform_real_distribution<float> distAngle(0.0f, 2.0f * glm::pi<float>());
	const float floorDiamAdj = (floorDiam - 1.0f) / 2.0f;

	float radius, angle;
	glm::vec3 offset;
	do {
		radius = distRadius(rng);
		angle = distAngle(rng);
		offset = glm::vec3(radius * cos(angle), 0.0f, radius * sin(angle));
	} while (glm::length(carPosition + offset) > floorDiamAdj);

	return carPosition + offset;
}

void Application::update_mike_positions(const glm::vec3& carPosition, std::vector<MikeInstance>& mikes, float& mikeSpawnTimer, const float deltaT, std::mt19937& rng, const float minRadius, const float maxRadius, const float floorDiam) {
	// Update Mike instances
	mikeSpawnTimer += deltaT;
	if (mikeSpawnTimer >= 1.0f) {
		mikeSpawnTimer -= 1.0f;
		for (auto& mike : mikes) {
			if (!mike.isAboveFloor) {
				mike.position = generateRandomPosition(carPosition, minRadius, maxRadius, rng, floorDiam); // Generate random position within floor boundaries
				mike.isAboveFloor = true;
				break;
			}
		}
	}

	for (auto& mike : mikes)
	{
		if (mike.isAboveFloor)
		{
			if (!mike.isDamaged)
			{
				glm::vec3 dirToPlayer = glm::normalize(carPosition - mike.position);
				mike.position += dirToPlayer * 2.0f * deltaT;
				mike.rotation = std::atan2(dirToPlayer.x, dirToPlayer.z);
			}
			if (mike.isDamaged)
				mike.damageTimer += deltaT;
			if (mike.damageTimer >= 0.2)
			{
				mike.isDamaged = false;
				mike.isAboveFloor = false;
				mike.position.y = -2.0;
				mike.damageTimer = 0.0;
			}
		}
	}
}

void Application::check_collisions_MB(std::vector<MikeInstance>& mikes, std::vector<Bullet>& bullets)
{
	for (auto& mike : mikes)
	{
		for (auto& bullet : bullets)
		{
			if (mike.isAboveFloor && bullet.getIsAboveFloor())
			{
				if (glm::distance(mike.position, bullet.getPosition()) <= 0.5)
				{
					mike.isDamaged = true;
					mike.damageTimer = 0.0;
					bullet.explode();
				}
			}
		}
	}
}

bool Application::check_collisions_MC(std::vector<MikeInstance>& mikes, glm::vec3& CarPosition)
{
	for (auto& mike : mikes)
	{
		if (glm::distance(mike.position, CarPosition) <= 0.5)
		{
			mike.isDamaged = false;
			mike.isAboveFloor = false;
			mike.position.y = -2.0;
			mike.damageTimer = 0.0;
			return true;
		}
	}
	return false;
}