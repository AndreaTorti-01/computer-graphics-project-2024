#include "Application.hpp"

// Window parameters setup
void Application::setWindowParameters()
{
	windowWidth = 1280;
	windowHeight = 720;
	windowTitle = "CG Project";
	windowResizable = GLFW_TRUE;
	initialBackgroundColor = { 0.1f, 0.1f, 0.1f, 1.0f };
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

	// Grass
	totalUniformBlocks += 1;
	totalTextures += 1;
	totalSets += 1;

	// Trophy
	totalUniformBlocks += 1;
	totalTextures += 3;
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
	// Initialize Descriptor Set Layouts
	DSLGlobal.init(this, { {0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_ALL_GRAPHICS, sizeof(GlobalUniformBufferObject), 1} });

	DSLToon.init(this, { {0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_ALL_GRAPHICS, sizeof(ToonUniformBufferObject), 1},
						{1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT, 0, 1},
						{2, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_FRAGMENT_BIT, sizeof(ToonParUniformBufferObject), 1},
						{3, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_ALL_GRAPHICS, sizeof(GlobalUniformBufferObject), 1} });

	DSLMike.init(this, { {0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_ALL_GRAPHICS, sizeof(MikeUniformBufferObject), 1},
						{1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT, 0, 1},
						{2, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_ALL_GRAPHICS, sizeof(GlobalUniformBufferObject), 1} });

	DSLTrophy.init(this, { {0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_ALL_GRAPHICS, sizeof(TrophyUniformBufferObject), 1},
						  {1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT, 0, 1},
						  {2, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT, 1, 1},
						  {3, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT, 2, 1} });

	DSLSkyBox.init(this, { {0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT, sizeof(SkyBoxUniformBufferObject), 1},
						  {1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT, 0, 1} });

	DSLTitles.init(this, { {0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_ALL_GRAPHICS, sizeof(ToonUniformBufferObject), 1},
						  {1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT, 0, 1} });

	// Initialize Vertex Descriptors
	VDGeneric.init(this, { {0, sizeof(GenericVertex), VK_VERTEX_INPUT_RATE_VERTEX} }, { {0, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(GenericVertex, pos), sizeof(glm::vec3), POSITION}, {0, 1, VK_FORMAT_R32G32B32_SFLOAT, offsetof(GenericVertex, norm), sizeof(glm::vec3), NORMAL}, {0, 2, VK_FORMAT_R32G32_SFLOAT, offsetof(GenericVertex, UV), sizeof(glm::vec2), UV} });
	VDSkyBox.init(this, { {0, sizeof(SkyBoxVertex), VK_VERTEX_INPUT_RATE_VERTEX} }, { {0, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(SkyBoxVertex, pos), sizeof(glm::vec3), POSITION} });

	// Pipelines [Shader couples]
	PToon.init(this, &VDGeneric, "shaders/Vert.spv", "shaders/ToonFrag.spv", { &DSLGlobal, &DSLToon });
	PMike.init(this, &VDGeneric, "shaders/MikeVert.spv", "shaders/MikeFrag.spv", { &DSLGlobal, &DSLMike });
	PTitles.init(this, &VDGeneric, "shaders/TitleVert.spv", "shaders/TitleFrag.spv", { &DSLTitles });
	PTrophy.init(this, &VDGeneric, "shaders/TrophyVert.spv", "shaders/TrophyFrag.spv", { &DSLTrophy });
	PSkyBox.init(this, &VDSkyBox, "shaders/SkyBoxVert.spv", "shaders/SkyBoxFrag.spv", { &DSLSkyBox });
	PSkyBox.setAdvancedFeatures(VK_COMPARE_OP_LESS_OR_EQUAL, VK_POLYGON_MODE_FILL, VK_CULL_MODE_BACK_BIT, false);

	// Create Models
	MCar.init(this, &VDGeneric, "models/CarHighPoly.obj", OBJ);
	MMike.init(this, &VDGeneric, "models/Mike.mgcg", MGCG);
	MSkyBox.init(this, &VDSkyBox, "models/SkyBox.obj", OBJ);
	MFloor.init(this, &VDGeneric, "models/squarefloor128.obj", OBJ);
	MGrass.init(this, &VDGeneric, "models/outergrass16.obj", OBJ);
	MFence.init(this, &VDGeneric, "models/Fence.obj", OBJ);
	MBullet.init(this, &VDGeneric, "models/Bullet.obj", OBJ);
	MUpgrade.init(this, &VDGeneric, "models/Upgrade.obj", OBJ);
	MTitle1.init(this, &VDGeneric, "models/Title1.obj", OBJ);
	MTitle2.init(this, &VDGeneric, "models/Title2.obj", OBJ);
	MTrophy.init(this, &VDGeneric, "models/Trophy.obj", OBJ);

	// Initialize Textures
	TGeneric.init(this, "textures/Textures.png");
	TMike.init(this, "textures/T_Mike.png");
	TSkyBox.init(this, "textures/T_SkyBox.png");
	TFloor.init(this, "textures/T_Floor.jpg");
	TCar.init(this, "textures/T_Car.png");
	TBullet.init(this, "textures/Textures.png");
	TUpgrade.init(this, "textures/Textures.png");
	TTitle1.init(this, "textures/T_Titles.png");
	TTitle2.init(this, "textures/T_Titles.png");
	TGrass.init(this, "textures/grass.jpg");
	TFence.init(this, "textures/T_Fence.jpg");
	TTrophy1.init(this, "textures/T_Trophy1.png");
	TTrophy2.init(this, "textures/T_Trophy2.png");
	TTrophy3.init(this, "textures/T_Trophy3.png");

	calculateDescriptorPoolSizes();

	// Initialize text
	outText.resize(3);
	outText = {
		{3, {"Press SPACE to start", "", "", ""}, 0, 0},
		{2, {"Evade the mikes", "And kill em all", "", ""}, 0, 0},
		{1, {"press ESC to close", "", "", ""}, 0, 0} };
	txt.init(this, &outText);

	// Initialize Ubos
	initConstantUbos();
	// Initialize matrices
	ViewMat = glm::translate(glm::mat4(1), -CamPos);
	cameraType = 0;
}

// Initialize pipelines and descriptor sets
void Application::pipelinesAndDescriptorSetsInit()
{
	// Create pipelines
	PMike.create();
	PToon.create();
	PSkyBox.create();
	PTitles.create();
	PTrophy.create();

	// Initialize descriptor sets
	DSCar.init(this, &DSLToon, { &TCar });
	DSMikes.init(this, &DSLMike, { &TGeneric });
	DSBullets.resize(MAX_BULLET_INSTANCES);
	for (int i = 0; i < MAX_BULLET_INSTANCES; ++i)
	{
		DSBullets[i].init(this, &DSLToon, { &TBullet });
	}
	DSUpgrades.resize(MAX_UPGRADE_INSTANCES);
	for (int i = 0; i < MAX_UPGRADE_INSTANCES; ++i)
	{
		DSUpgrades[i].init(this, &DSLToon, { &TUpgrade });
	}
	DSFloor.init(this, &DSLToon, { &TFloor });
	DSGrass.init(this, &DSLToon, { &TGrass });
	DSFence.init(this, &DSLToon, { &TFence });
	DSSkyBox.init(this, &DSLSkyBox, { &TSkyBox });
	DSTitle1.init(this, &DSLTitles, { &TTitle1 });
	DSTitle2.init(this, &DSLTitles, { &TTitle2 });
	DSTrophy.init(this, &DSLTrophy, { &TTrophy1, &TTrophy2, &TTrophy3 });
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
	PTitles.cleanup();
	PTrophy.cleanup();

	DSCar.cleanup();
	DSMikes.cleanup();
	for (auto& ds : DSBullets)
	{
		ds.cleanup();
	}
	for (auto& ds : DSUpgrades)
	{
		ds.cleanup();
	}
	DSFloor.cleanup();
	DSGrass.cleanup();
	DSFence.cleanup();
	DSSkyBox.cleanup();
	DSTitle1.cleanup();
	DSTitle2.cleanup();
	DSTrophy.cleanup();
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
	TUpgrade.cleanup();
	TTitle1.cleanup();
	TTitle2.cleanup();
	TGrass.cleanup();
	TFence.cleanup();
	TTrophy1.cleanup();
	TTrophy2.cleanup();
	TTrophy3.cleanup();

	MCar.cleanup();
	MMike.cleanup();
	MSkyBox.cleanup();
	MFloor.cleanup();
	MBullet.cleanup();
	MUpgrade.cleanup();
	MTitle1.cleanup();
	MTitle2.cleanup();
	MGrass.cleanup();
	MFence.cleanup();
	MTrophy.cleanup();

	DSLToon.cleanup();
	DSLMike.cleanup();
	DSLSkyBox.cleanup();
	DSLGlobal.cleanup();
	DSLTitles.cleanup();
	DSLTrophy.cleanup();

	PToon.destroy();
	PMike.destroy();
	PSkyBox.destroy();
	PTitles.destroy();
	PTrophy.destroy();

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

	// Render Grass
	PToon.bind(commandBuffer);
	MGrass.bind(commandBuffer);
	DSGlobal.bind(commandBuffer, PToon, 0, currentImage);
	DSGrass.bind(commandBuffer, PToon, 1, currentImage);
	vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(MGrass.indices.size()), 1, 0, 0, 0);

	// Render Fence
	PToon.bind(commandBuffer);
	MFence.bind(commandBuffer);
	DSGlobal.bind(commandBuffer, PToon, 0, currentImage);
	DSFence.bind(commandBuffer, PToon, 1, currentImage);
	vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(MFence.indices.size()), 1, 0, 0, 0);

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
	for (int i = 0; i < MAX_BULLET_INSTANCES; ++i)
	{
		DSBullets[i].bind(commandBuffer, PToon, 1, currentImage);
		vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(MBullet.indices.size()), 1, 0, 0, 0);
	}

	// Render Upgrades instances
	PToon.bind(commandBuffer);
	MUpgrade.bind(commandBuffer);
	DSGlobal.bind(commandBuffer, PToon, 0, currentImage);
	for (int i = 0; i < MAX_UPGRADE_INSTANCES; ++i)
	{
		DSUpgrades[i].bind(commandBuffer, PToon, 1, currentImage);
		vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(MUpgrade.indices.size()), 1, 0, 0, 0);
	}

	// Render Titles
	PTitles.bind(commandBuffer);
	MTitle1.bind(commandBuffer);
	DSTitle1.bind(commandBuffer, PTitles, 0, currentImage);
	vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(MTitle1.indices.size()), 1, 0, 0, 0);

	PTitles.bind(commandBuffer);
	MTitle2.bind(commandBuffer);
	DSTitle2.bind(commandBuffer, PTitles, 0, currentImage);
	vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(MTitle2.indices.size()), 1, 0, 0, 0);

	// Render Trophy
	PTrophy.bind(commandBuffer);
	MTrophy.bind(commandBuffer);
	DSTrophy.bind(commandBuffer, PTrophy, 0, currentImage);
	vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(MTrophy.indices.size()), 1, 0, 0, 0);

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
		setScene0(currentImage);
		if (glfwGetKey(window, GLFW_KEY_SPACE))
		{
			currScene = 1;
			RebuildPipeline();
		}
		return;
	}
	if (currScene == 1)
	{
		setScene1(currentImage);
		if (glfwGetKey(window, GLFW_KEY_ESCAPE))
		{
			glfwSetWindowShouldClose(window, 1);
			return;
		}
		return;
	}
	if (currScene == 2)
	{
		setScene2(currentImage);

		if (glfwGetKey(window, GLFW_KEY_ESCAPE))
		{
			glfwSetWindowShouldClose(window, 1);
		}
		return;
	}
}

void Application::initConstantUbos()
{
	constexpr float fov = glm::radians(45.0f);
	glm::mat4 M = glm::perspective(fov, Ar, 0.1f, 500.0f);
	M[1][1] *= -1; // Flip Y-axis for Vulkan coordinate
	TitleViewMatrix = glm::lookAt(defaultEyePos, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	TitleViewPrj = M * TitleViewMatrix;
}

void Application::setScene0(uint32_t currentImage)
{
	timeManager.update();
	float passedT = timeManager.getPassedTime();

	tubo.mMat = glm::translate(glm::mat4(5.0f), glm::vec3(0.0f, 5.0f, 0.0f));
	tubo.mvpMat = TitleViewPrj * tubo.mMat;
	tubo.nMat = glm::inverse(glm::transpose(tubo.mMat));

	DSTitle1.map(currentImage, &tubo, 0);

	// Update Skybox uniforms
	SkyBoxUniformBufferObject uboSky{};
	uboSky.mvpMat = glm::scale(TitleViewPrj, glm::vec3(50.0f)); // Remove translation from view matrix
	uboSky.mvpMat = glm::rotate(uboSky.mvpMat, glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	DSSkyBox.map(currentImage, &uboSky, 0);

	ToonUniformBufferObject uboCar{};
	ToonParUniformBufferObject uboToonParC{};
	uboToonParC.textureMultiplier = 1.0f;
	uboCar.mMat = glm::translate(glm::mat4(3.0f), glm::vec3(0.0f, -40.0f, 0.0f));
	uboCar.mMat = glm::rotate(uboCar.mMat, glm::radians(-60.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	uboCar.mMat = glm::rotate(uboCar.mMat, glm::radians(90.0f) * passedT, glm::vec3(0.0f, 1.0f, 0.0f));

	uboCar.mvpMat = TitleViewPrj * uboCar.mMat;
	uboCar.nMat = glm::inverse(uboCar.mMat);

	DSCar.map(currentImage, &uboCar, 0);
	DSCar.map(currentImage, &uboToonParC, 2);

	GlobalUniformBufferObject uboGlobal;
	uboGlobal.eyePos = glm::vec3(glm::inverse(TitleViewMatrix) * glm::vec4(0, 0, 0, 1));
	uboGlobal.lightColor[0].v = glm::vec4(0.5f);
	uboGlobal.lightPos[0].v = glm::vec3(0.0f);
	uboGlobal.lightDir[0].v = glm::vec3(0.0f, 1.0f, 0.0f);
	uboGlobal.type[0].t = 0;

	for (int i = 0; i < NLIGHTS - 1; i++)
	{
		uboGlobal.lightDir[i + 1].v = glm::vec3(0.0f);
		uboGlobal.lightPos[i + 1].v = glm::vec3(0.0f);
		uboGlobal.lightColor[i + 1].v = glm::vec4(0.0f);
		uboGlobal.type[i + 1].t = 1;
	}
	DSGlobal.map(currentImage, &uboGlobal, 0);
}

void Application::setScene2(uint32_t currentImage)
{
	std::cout << "Score: " << score << std::endl;

	timeManager.update();
	float passedT = timeManager.getPassedTime();

	tubo.mMat = glm::mat4(5.0f);
	// tubo.mMat = glm::rotate(tubo.mMat, glm::radians(45.0f) * passedT, glm::vec3(0.0f, 0.0f, 1.0f));
	tubo.mvpMat = TitleViewPrj * tubo.mMat;
	tubo.nMat = glm::inverse(glm::transpose(tubo.mMat));

	DSTitle2.map(currentImage, &tubo, 0);

	// Update Skybox uniforms
	SkyBoxUniformBufferObject uboSky{};
	uboSky.mvpMat = glm::scale(TitleViewPrj, glm::vec3(50.0f)); // Remove translation from view matrix
	uboSky.mvpMat = glm::rotate(uboSky.mvpMat, glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	DSSkyBox.map(currentImage, &uboSky, 0);

	TrophyUniformBufferObject uboTrophy{};
	uboTrophy.mMat = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -30.0f, 0.0f));
	uboTrophy.mMat = glm::scale(uboTrophy.mMat, glm::vec3(5.0f));
	uboTrophy.mMat = glm::rotate(uboTrophy.mMat, glm::radians(30.0f) * passedT, glm::vec3(0.0f, 0.0f, 1.0f));
	uboTrophy.mMat = glm::rotate(uboTrophy.mMat, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	uboTrophy.mvpMat = TitleViewPrj * uboTrophy.mMat;
	uboTrophy.nMat = glm::inverse(glm::transpose(tubo.mMat));
	uboTrophy.prize = 0;
	score = car.getScore();

	if (score > 0)
		uboTrophy.prize = 3;
	if (score > 1)
		uboTrophy.prize = 2;
	if (score > 2)
		uboTrophy.prize = 1;

	if (score > 0)
		DSTrophy.map(currentImage, &uboTrophy, 0);
}

void Application::setScene1(uint32_t currentImage)
{
	float deltaT;
	glm::vec3 m = glm::vec3(0.0f), r = glm::vec3(0.0f);
	bool fire = false;

	timeManager.update();
	timeManager.updateTimers(car, mikes, upgrades);
	deltaT = timeManager.getDeltaTime();

	getSixAxis(deltaT, m, r, fire);

	car.update(deltaT, m);
	for (auto& up : upgrades)
		up.update(deltaT);
	for (auto& mike : mikes)
		mike.update(deltaT, car.getPosition());

	car.check_collisions(mikes, upgrades);
	if (car.getHealth() <= 0)
	{
		RebuildPipeline();
		currScene = 2;
		return;
	}
	if (glfwGetKey(window, GLFW_KEY_SPACE))
	{
		if (timeManager.canChangeView())
			cameraType = (cameraType + 1) % 3;
		glfwWaitEventsTimeout(0.2);
	}
	glm::vec3 cameraOffset;
	// Camera position (static relative to the car)
	switch (cameraType)
	{
	case 0:
		cameraOffset = glm::vec3(5.0f, 20.0f, 5.0f);
		break;
	case 1:
		cameraOffset = glm::vec3(15.0f, 20.0f, 15.0f);
		break;
	default: cameraOffset = glm::vec3(15.0f, 20.0f, 15.0f);
	}

	if (cameraType < 2)
	{
		glm::vec3 cameraPosition = car.getPosition() + cameraOffset;
		// Look at the car
		ViewMat = glm::lookAt(cameraPosition, car.getPosition(), glm::vec3(0.0f, 1.0f, 0.0f));
	}
	else
	{
		ViewMat =
			glm::rotate(glm::mat4(1.0), -glm::radians(-5.0f), glm::vec3(1, 0, 0)) *
			glm::rotate(glm::mat4(1.0), car.getRotation() + glm::radians(-90.0f), glm::vec3(0, 1, 0)) *
			glm::translate(glm::mat4(1.0), -car.getPosition()) *
			glm::translate(glm::mat4(1.0), glm::vec3(-0.0f, -0.8f, -0.0f));
	}

	// Projection matrix with FOV adjustment based on speed
	float fov = glm::radians(30.0f + std::abs(car.getSpeed()) * 0.75f);
	glm::mat4 ProjectionMat = glm::perspective(fov, Ar, 0.1f, 160.0f);
	ProjectionMat[1][1] *= -1; // Flip Y-axis for Vulkan coordinate

	glm::mat4 ViewPrjMat = ProjectionMat * ViewMat;

	// Shake camera slightly at high speed
	if (std::abs(car.getSpeed()) > 0.9f * MAX_SPEED)
	{
		float shakeAmount = 0.004f * (std::abs(car.getSpeed()) - 0.9f * MAX_SPEED) / (0.1f * MAX_SPEED);
		ViewPrjMat = glm::translate(ViewPrjMat, glm::vec3(
			0.0f,
			shakeAmount * (std::sin(deltaT * 50.0f) + std::cos(deltaT * 47.0f)),
			shakeAmount * (std::cos(deltaT * 53.0f) + std::sin(deltaT * 59.0f))));
	}

	// Update global uniforms
	GlobalUniformBufferObject uboGlobal{};
	uboGlobal.eyePos = glm::vec3(glm::inverse(ViewMat) * glm::vec4(0, 0, 0, 1));

	uboGlobal.lightPos[0].v = glm::vec3(0.0f);
	uboGlobal.lightDir[0].v = glm::vec3(0.0f, 1.0f, 0.0f);
	uboGlobal.lightColor[0].v = glm::vec4(0.1f * car.getHealth());
	uboGlobal.type[0].t = 0;

	for (int i = 0; i < NLIGHTS - 1; i++)
	{
		uboGlobal.lightDir[i + 1].v = glm::vec3(0.0f, 0.0f, 0.0f);
		uboGlobal.lightPos[i + 1].v = mikes[i].getPosition() + glm::vec3(0.0f, 1.0f, 0.0f);
		uboGlobal.lightColor[i + 1].v = glm::vec4(0.8f);
		uboGlobal.type[i + 1].t = 1;
	}
	DSGlobal.map(currentImage, &uboGlobal, 0);

	// Update Car uniforms
	ToonUniformBufferObject uboCar{};
	ToonParUniformBufferObject uboToonParC{};
	uboToonParC.textureMultiplier = 1.0f;
	uboCar.mMat = car.getPosition4(); // local transform ("model" matrix) - matrix is ready in car object
	uboCar.mvpMat = ViewPrjMat * uboCar.mMat; // model-view-projection matrix
	uboCar.nMat = glm::inverse(uboCar.mMat); // normal matrix
	DSCar.map(currentImage, &uboCar, 0);
	DSCar.map(currentImage, &uboToonParC, 2);

	// Update Mike uniforms
	MikeUniformBufferObject uboMike{};
	for (int i = 0; i < mikes.size(); i++)
	{
		glm::mat4 rotationMat = glm::rotate(glm::mat4(1.0f), mikes[i].getRotation(), glm::vec3(0.0f, 1.0f, 0.0f));
		uboMike.mMat[i] = glm::translate(glm::mat4(1.0f), mikes[i].getPosition()) * rotationMat; // 2nd case: matrix is created here
		uboMike.mMat[i] = glm::scale(uboMike.mMat[i], glm::vec3(0.5f));
		uboMike.mvpMat[i] = ViewPrjMat * uboMike.mMat[i];
		uboMike.nMat[i] = glm::inverse(glm::transpose(uboMike.mMat[i]));
		// uboMike.showDamage[i] = 0;
		uboMike.showDamage[i].s = mikes[i].getDamaged() ? 1 : 0;
	}
	DSMikes.map(currentImage, &uboMike, 0);

	// Update Bullet uniforms
	for (int i = 0; i < car.getBullets().size(); ++i)
	{
		ToonUniformBufferObject uboBullet{};
		ToonParUniformBufferObject uboToonPar{};
		uboToonPar.textureMultiplier = 1.0f;
		glm::mat4 rotationMat = glm::rotate(glm::mat4(1.0f), car.getBullets()[i].getRotation(), glm::vec3(0.0f, 1.0f, 0.0f));
		uboBullet.mMat = glm::translate(glm::mat4(1.0f), car.getBullets()[i].getPosition()) * rotationMat;
		uboBullet.mvpMat = ViewPrjMat * uboBullet.mMat;
		uboBullet.nMat = glm::inverse(glm::transpose(uboBullet.mMat));
		DSBullets[i].map(currentImage, &uboBullet, 0);
		DSBullets[i].map(currentImage, &uboToonPar, 2);
	}

	// Update Upgrade uniforms
	for (int i = 0; i < upgrades.size(); ++i)
	{
		ToonUniformBufferObject uboUpgrade{};
		ToonParUniformBufferObject uboToonParU{};
		uboToonParU.textureMultiplier = 1.0f;
		uboUpgrade.mMat = upgrades[i].getPosition4();
		uboUpgrade.mvpMat = ViewPrjMat * uboUpgrade.mMat;
		uboUpgrade.nMat = glm::inverse(glm::transpose(uboUpgrade.mMat));
		DSUpgrades[i].map(currentImage, &uboUpgrade, 0);
		DSUpgrades[i].map(currentImage, &uboToonParU, 2);
	}

	// Update Floor uniforms
	ToonUniformBufferObject uboFloor{};
	ToonParUniformBufferObject uboToonParF{};
	uboToonParF.textureMultiplier = FLOOR_DIAM / 32.0;
	uboFloor.mMat = glm::mat4(1.0f);
	uboFloor.mvpMat = ViewPrjMat * uboFloor.mMat;
	uboFloor.nMat = glm::transpose(glm::inverse(uboFloor.mMat));

	DSFloor.map(currentImage, &uboFloor, 0);
	DSFloor.map(currentImage, &uboToonParF, 2);

	// Update Grass uniforms
	ToonUniformBufferObject uboGrass{};
	ToonParUniformBufferObject uboToonParG{};
	uboToonParG.textureMultiplier = FLOOR_DIAM / 4.0; // Adjust this to control texture tiling
	uboGrass.mMat = glm::mat4(1.0f);
	uboGrass.mvpMat = ViewPrjMat * uboGrass.mMat;
	uboGrass.nMat = glm::transpose(glm::inverse(uboGrass.mMat));

	DSGrass.map(currentImage, &uboGrass, 0);
	DSGrass.map(currentImage, &uboToonParG, 2);

	// Update Fence uniforms
	ToonUniformBufferObject uboFence{};
	ToonParUniformBufferObject uboToonParFe{};
	uboToonParFe.textureMultiplier = FLOOR_DIAM / 32.0f;
	uboFence.mMat = glm::mat4(1.0f);
	uboFence.mvpMat = ViewPrjMat * uboFence.mMat;
	uboFence.nMat = glm::transpose(glm::inverse(uboFence.mMat));

	DSFence.map(currentImage, &uboFence, 0);
	DSFence.map(currentImage, &uboToonParFe, 2);

	// Update Skybox uniforms
	SkyBoxUniformBufferObject uboSky{};
	uboSky.mvpMat = ProjectionMat * glm::mat4(glm::mat3(ViewMat)); // Remove translation from view matrix
	DSSkyBox.map(currentImage, &uboSky, 0);
}