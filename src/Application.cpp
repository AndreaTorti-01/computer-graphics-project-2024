#include "Application.hpp"

// Window parameters setup
void Application::setWindowParameters()
{
	windowWidth = 800;
	windowHeight = 600;
	windowTitle = "CG Project";
	windowResizable = GLFW_TRUE;
	initialBackgroundColor = {0.1f, 0.1f, 0.1f, 1.0f};
	Ar = static_cast<float>(windowWidth) / static_cast<float>(windowHeight);
}

// Window resize handler
void Application::onWindowResize(int w, int h)
{
	std::cout << "Window resized to: " << w << " x " << h << "\n";
	Ar = static_cast<float>(w) / static_cast<float>(h);
}

// Add this new function to calculate descriptor pool sizes
void Application::calculateDescriptorPoolSizes()
{
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
	DPSZs.uniformBlocksInPool = totalUniformBlocks * 10;
	DPSZs.texturesInPool = totalTextures * 10;
	DPSZs.setsInPool = totalSets * 10;
}

// Initialization of local resources
void Application::localInit()
{
	Ar = static_cast<float>(windowWidth) / static_cast<float>(windowHeight);

	// Initialize Descriptor Set Layouts
	DSLGlobal.init(this, {{0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_ALL_GRAPHICS, sizeof(GlobalUniformBufferObject), 1}});
	DSLToon.init(this, {{0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_ALL_GRAPHICS, sizeof(ToonUniformBufferObject), 1},
						{1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT, 0, 1},
						{2, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_FRAGMENT_BIT, sizeof(ToonParUniformBufferObject), 1},
						{3, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_ALL_GRAPHICS, sizeof(GlobalUniformBufferObject), 1}});

	DSLMike.init(this, {{0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_ALL_GRAPHICS, sizeof(MikeUniformBufferObject), 1},
					  {1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT, 0, 1},
					  {2, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT, sizeof(MikeParUniformBufferObject), 1},
					  {3, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_ALL_GRAPHICS, sizeof(GlobalUniformBufferObject), 1}});

	DSLSkyBox.init(this, {{0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT, sizeof(SkyBoxUniformBufferObject), 1},
						  {1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT, 0, 1}});

	// Initialize Vertex Descriptors
	VDGeneric.init(this, {{0, sizeof(GenericVertex), VK_VERTEX_INPUT_RATE_VERTEX}}, {{0, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(GenericVertex, pos), sizeof(glm::vec3), POSITION}, {0, 1, VK_FORMAT_R32G32B32_SFLOAT, offsetof(GenericVertex, norm), sizeof(glm::vec3), NORMAL}, {0, 2, VK_FORMAT_R32G32_SFLOAT, offsetof(GenericVertex, UV), sizeof(glm::vec2), UV}});
	VDSkyBox.init(this, {{0, sizeof(SkyBoxVertex), VK_VERTEX_INPUT_RATE_VERTEX}}, {{0, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(SkyBoxVertex, pos), sizeof(glm::vec3), POSITION}});

	// Pipelines [Shader couples]
	PToon.init(this, &VDGeneric, "shaders/Vert.spv", "shaders/ToonFrag.spv", {&DSLGlobal, &DSLToon});
	PMike.init(this, &VDGeneric, "shaders/MikeVert.spv", "shaders/MikeFrag.spv", {&DSLGlobal, &DSLMike});
	PSkyBox.init(this, &VDSkyBox, "shaders/SkyBoxVert.spv", "shaders/SkyBoxFrag.spv", {&DSLSkyBox});
	PSkyBox.setAdvancedFeatures(VK_COMPARE_OP_LESS_OR_EQUAL, VK_POLYGON_MODE_FILL, VK_CULL_MODE_BACK_BIT, false);

	// Create Models
	MCar.init(this, &VDGeneric, "models/Car.obj", OBJ);
	MMike.init(this, &VDGeneric, "models/Mike.mgcg", MGCG);
	MSkyBox.init(this, &VDSkyBox, "models/SkyBox.obj", OBJ);
	MFloor.init(this, &VDGeneric, "models/newFloor.obj", OBJ);
	MBullet.init(this, &VDGeneric, "models/Bullet.obj", OBJ);

	// Initialize Textures
	TGeneric.init(this, "textures/Textures.png");
	TMike.init(this, "textures/T_Mike.png");
	TSkyBox.init(this, "textures/T_SkyBox.jpg");
	TFloor.init(this, "textures/TCom_Pavement_TerracottaAntique_2K_albedo.jpg");
	TCar.init(this, "textures/T_Car.jpg");
	TBullet.init(this, "textures/Textures.png");

	calculateDescriptorPoolSizes();

	// Initialize text
	outText.resize(3);
	outText = {
		{3, {"Press SPACE to start", " ", "", ""}, 0, 0},
		{2, {"Evade the mikes", "And kill em all", "", ""}, 0, 0},
		{1, {"GAME OVER", "press ESC to close", "", ""}, 0, 0}};
	txt.init(this, &outText);

	// Initialize matrices
	ViewMatrix = glm::translate(glm::mat4(1), -CamPos);
}

// Initialize pipelines and descriptor sets
void Application::pipelinesAndDescriptorSetsInit()
{
	// Create pipelines
	PMike.create();
	PToon.create();
	PSkyBox.create();

	// Initialize descriptor sets
	DSCar.init(this, &DSLToon, {&TGeneric});
	DSMikes.init(this, &DSLMike, {&TGeneric});
	DSBullets.resize(MAX_BULLET_INSTANCES);
	for (int i = 0; i < MAX_BULLET_INSTANCES; ++i)
	{
		DSBullets[i].init(this, &DSLToon, {&TBullet});
	}
	DSFloor.init(this, &DSLToon, {&TFloor});
	DSSkyBox.init(this, &DSLSkyBox, {&TSkyBox});
	DSGlobal.init(this, &DSLGlobal, {});

	// Initialize text pipelines and descriptor sets
	txt.pipelinesAndDescriptorSetsInit();
}

// Cleanup pipelines and descriptor sets
void Application::pipelinesAndDescriptorSetsCleanup()
{
	PToon.cleanup();
	PSkyBox.cleanup();
	PMike.cleanup();

	DSCar.cleanup();
	DSMikes.cleanup();
	for (auto &ds : DSBullets)
	{
		ds.cleanup();
	}
	DSFloor.cleanup();
	DSSkyBox.cleanup();
	DSGlobal.cleanup();

	txt.pipelinesAndDescriptorSetsCleanup();
}

// Cleanup local resources
void Application::localCleanup()
{
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
	DSLMike.cleanup();
	DSLSkyBox.cleanup();
	DSLGlobal.cleanup();

	PToon.destroy();
	PMike.destroy();
	PSkyBox.destroy();

	txt.localCleanup();
}

// Populate command buffer for rendering
void Application::populateCommandBuffer(VkCommandBuffer commandBuffer, int currentImage)
{
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
	PMike.bind(commandBuffer);
	MMike.bind(commandBuffer);
	DSGlobal.bind(commandBuffer, PMike, 0, currentImage);
	DSMikes.bind(commandBuffer, PMike, 1, currentImage);
	vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(MMike.indices.size()), MAX_MIKE_INSTANCES, 0, 0, 0);

	// Render Bullet instances
	PToon.bind(commandBuffer);
	MBullet.bind(commandBuffer);
	DSGlobal.bind(commandBuffer, PToon, 0, currentImage);
	for (size_t i = 0; i < MAX_BULLET_INSTANCES; ++i)
	{
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
void Application::updateUniformBuffer(uint32_t currentImage)
{

	if (currScene == 0)
	{
		if (glfwGetKey(window, GLFW_KEY_SPACE))
		{
			currScene = 1;
			RebuildPipeline();
		}
		return;
	}
	if (currScene == 2)
	{
		if (glfwGetKey(window, GLFW_KEY_ESCAPE))
		{
			glfwSetWindowShouldClose(window, 1);
		}
		return;
	}

	float deltaT;
	glm::vec3 m = glm::vec3(0.0f), r = glm::vec3(0.0f);
	bool fire = false;

	if (glfwGetKey(window, GLFW_KEY_ESCAPE))
	{
		glfwSetWindowShouldClose(window, 1);
		return;
	}
	if (glfwGetKey(window, GLFW_KEY_SPACE))
	{
		isIsometricView = !isIsometricView;
		glfwWaitEventsTimeout(0.2);
	}

	timeManager.update();
	timeManager.updateTimers(car, mikes);
	deltaT = timeManager.getDeltaTime();

	getSixAxis(deltaT, m, r, fire);

	car.update(deltaT, m);
	for (auto &mike : mikes)
		mike.update(deltaT, car.getPosition());

	car.check_collisions(mikes);
	if (car.getHealth() <= 0)
	{
		RebuildPipeline();
		currScene = 2;
	}

	// Camera position (static relative to the car)
	glm::vec3 cameraOffset = glm::vec3(5.0f, 20.0f, 5.0f);
	if (isIsometricView)
		cameraOffset = glm::vec3(15.0f, 20.0f, 15.0f);
	glm::vec3 cameraPosition = car.getPosition() + cameraOffset;

	// Look at the car
	ViewMatrix = glm::lookAt(cameraPosition, car.getPosition(), glm::vec3(0.0f, 1.0f, 0.0f));

	// Projection matrix with FOV adjustment based on speed
	float fov = glm::radians(30.0f + std::abs(car.getSpeed()) * 0.75f);
	glm::mat4 M = glm::perspective(fov, Ar, 0.1f, 160.0f);
	M[1][1] *= -1; // Flip Y-axis for Vulkan coordinate

	glm::mat4 ViewPrj = M * ViewMatrix;

	// Shake camera slightly at high speed
	if (std::abs(car.getSpeed()) > 0.9f * MAX_SPEED)
	{
		float shakeAmount = 0.004f * (std::abs(car.getSpeed()) - 0.9f * MAX_SPEED) / (0.1f * MAX_SPEED);
		ViewPrj = glm::translate(ViewPrj, glm::vec3(
											  0.0f,
											  shakeAmount * (std::sin(deltaT * 50.0f) + std::cos(deltaT * 47.0f)),
											  shakeAmount * (std::cos(deltaT * 53.0f) + std::sin(deltaT * 59.0f))));
	}

	// Update global uniforms
	GlobalUniformBufferObject uboGlobal{};
	uboGlobal.eyePos = glm::vec3(glm::inverse(ViewMatrix) * glm::vec4(0, 0, 0, 1));

	uboGlobal.lightPos[0].v = glm::vec3(0.0f);
	uboGlobal.lightDir[0].v = glm::vec3(0.0f, 1.0f, 0.0f);
	uboGlobal.lightColor[0].v = glm::vec4(0.6f);
	uboGlobal.type[0].t = 0.0f;

	for (int i = 0; i < NLIGHTS - 1; i++)
	{
		uboGlobal.lightDir[i + 1].v = glm::vec3(0.0f, 0.0f, 0.0f);

		uboGlobal.lightPos[i + 1].v = mikes[i].getPosition() + glm::vec3(0.0f, 1.0f, 0.0f);
		uboGlobal.lightColor[i + 1].v = glm::vec4(0.8f);
		uboGlobal.type[i + 1].t = 1.0f;
	}
	DSGlobal.map(currentImage, &uboGlobal, 0);

	// Update Car uniforms
	ToonUniformBufferObject uboCar{};
	ToonParUniformBufferObject uboToonParC{};
	uboToonParC.edgeDetectionOn = 1.0f;
	uboToonParC.textureMultiplier = 1.0f;
	uboCar.mMat = car.getPosition4();
	uboCar.mvpMat = ViewPrj * uboCar.mMat;
	uboCar.nMat = glm::inverse(uboCar.mMat);
	DSCar.map(currentImage, &uboCar, 0);
	DSCar.map(currentImage, &uboToonParC, 2);

	// Update Mike uniforms
	MikeUniformBufferObject uboMike{};
	MikeParUniformBufferObject uboMikePar{};
	for (size_t i = 0; i < mikes.size(); i++)
	{
		glm::mat4 rotationMat = glm::rotate(glm::mat4(1.0f), mikes[i].getRotation(), glm::vec3(0.0f, 1.0f, 0.0f));
		uboMike.mMat[i] = glm::translate(glm::mat4(1.0f), mikes[i].getPosition()) * rotationMat;
		uboMike.mMat[i] = glm::scale(uboMike.mMat[i], glm::vec3(0.5f));
		uboMike.mvpMat[i] = ViewPrj * uboMike.mMat[i];
		uboMike.nMat[i] = glm::inverse(glm::transpose(uboMike.mMat[i]));
		if(mikes[i].getDamaged()) uboMikePar.showDamage[i].p = 1.0f;
		else  uboMikePar.showDamage[i].p = 0.0f;
	}
	DSMikes.map(currentImage, &uboMike, 0);
	DSMikes.map(currentImage, &uboMikePar, 2);

	// Update Bullet uniforms
	for (size_t i = 0; i < car.getBullets().size(); ++i)
	{
		ToonUniformBufferObject uboBullet{};
		ToonParUniformBufferObject uboToonPar{};
		uboToonPar.edgeDetectionOn = 1.0f;
		uboToonPar.textureMultiplier = 1.0f;
		glm::mat4 rotationMat = glm::rotate(glm::mat4(1.0f), car.getBullets()[i].getRotation(), glm::vec3(0.0f, 1.0f, 0.0f));
		uboBullet.mMat = glm::translate(glm::mat4(1.0f), car.getBullets()[i].getPosition()) * rotationMat;
		uboBullet.mvpMat = ViewPrj * uboBullet.mMat;
		uboBullet.nMat = glm::inverse(glm::transpose(uboBullet.mMat));
		DSBullets[i].map(currentImage, &uboBullet, 0);
		DSBullets[i].map(currentImage, &uboToonPar, 2);
	}

	// Update Floor uniforms
	ToonUniformBufferObject uboFloor{};
	ToonParUniformBufferObject uboToonParF{};
	uboToonParF.edgeDetectionOn = 0.0f;
	uboToonParF.textureMultiplier = FLOOR_DIAM / 32.0;
	uboFloor.mMat = glm::scale(glm::mat4(1.0f), glm::vec3(FLOOR_DIAM));
	uboFloor.mvpMat = ViewPrj * uboFloor.mMat;
	uboFloor.nMat = glm::transpose(glm::inverse(uboFloor.mMat));

	DSFloor.map(currentImage, &uboFloor, 0);
	DSFloor.map(currentImage, &uboToonParF, 2);

	// Update Skybox uniforms
	SkyBoxUniformBufferObject uboSky{};
	uboSky.mvpMat = M * glm::mat4(glm::mat3(ViewMatrix)); // Remove translation from view matrix
	DSSkyBox.map(currentImage, &uboSky, 0);
}

glm::vec3 generateRandomPosition(Car car, const float minRadius, const float maxRadius, std::mt19937 &rng, const float floorDiam)
{
	std::uniform_real_distribution<float> distRadius(minRadius, maxRadius);
	std::uniform_real_distribution<float> distAngle(0.0f, 2.0f * glm::pi<float>());
	const float floorDiamAdj = (floorDiam - 1.0f) / 2.0f;

	float radius, angle;
	glm::vec3 offset;
	do
	{
		radius = distRadius(rng);
		angle = distAngle(rng);
		offset = glm::vec3(radius * cos(angle), 0.0f, radius * sin(angle));
	} while (glm::length(car.getPosition() + offset) > floorDiamAdj);

	return car.getPosition() + offset;
}
