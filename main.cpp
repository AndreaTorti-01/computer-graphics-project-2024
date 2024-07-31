// This has been adapted from the Vulkan tutorial

#include "modules/Starter.hpp"
#include "modules/TextMaker.hpp"


std::vector<SingleText> outText = {
	{2, {"Adding an object", "Press SPACE to save the screenshots","",""}, 0, 0},
	{1, {"Saving Screenshots. Please wait.", "", "",""}, 0, 0}
};

// The uniform buffer object used in this example
struct ToonUniformBufferObject {
	alignas(16) glm::mat4 mvpMat;
	alignas(16) glm::mat4 mMat;
	alignas(16) glm::mat4 nMat;
};

struct SkyBoxUniformBufferObject {
	alignas(16) glm::mat4 mvpMat;
};

struct GlobalUniformBufferObject {
	alignas(16) glm::vec3 lightDir;
	alignas(16) glm::vec4 lightColor;
	alignas(16) glm::vec3 eyePos;
};



// The vertices data structures
// Example
struct ToonVertex {
	glm::vec3 pos;
	glm::vec3 norm;
	glm::vec2 UV;
};

struct SkyBoxVertex {
	glm::vec3 pos;
};


// MAIN ! 
class Application : public BaseProject {
	protected:
	
	// Descriptor Layouts ["classes" of what will be passed to the shaders]
	DescriptorSetLayout DSLGlobal, DSLSkyBox, DSLToon, DSLBW;	// For Global values
	
  // Vertex formats
	VertexDescriptor VDToon, VDSkyBox;

	// Pipelines [Shader couples]
	Pipeline PToon, PBW, PSkyBox;

	// Scenes and texts
  TextMaker txt;

	// Models, textures and Descriptor Sets (values assigned to the uniforms)
  Model MCar, MMike, MSkyBox, TFloor;
  Texture TGeneric, TMike, TSkyBox;

  DescriptorSet DSGlobal, DSCar, DSMike, DSSkyBox, DSFloor;
	
	// Other application parameters
	int currScene = 0;
	int subpass = 0;
		
	glm::vec3 CamPos = glm::vec3(0.0, 1.5, 7.0);
	float CamAlpha = 0.0f;
	float CamBeta = 0.0f;
	float Ar;
	
	// Here you set the main application parameters
	void setWindowParameters() {
		// window size, titile and initial background
		windowWidth = 800;
		windowHeight = 600;
		windowTitle = "CG";
    	windowResizable = GLFW_TRUE;
		initialBackgroundColor = {0.1f, 0.1f, 0.1f, 1.0f};
		
		Ar = (float)windowWidth / (float)windowHeight;
	}
	
	// What to do when the window changes size
	void onWindowResize(int w, int h) {
		std::cout << "Window resized to: " << w << " x " << h << "\n";
		Ar = (float)w / (float)h;
	}
	
	// Here you load and setup all your Vulkan Models and Texutures.
	// Here you also create your Descriptor set layouts and load the shaders for the pipelines
	void localInit() {
		// Descriptor Layouts [what will be passed to the shaders]
		DSLGlobal.init(this, {
					{0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_ALL_GRAPHICS, sizeof(GlobalUniformBufferObject), 1}
				});

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
					{1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT, 0, 1},
					{2, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT, 1, 1}
				  });


		// Vertex descriptors
    //
		VDToon.init(this, {
				  {0, sizeof(ToonVertex), VK_VERTEX_INPUT_RATE_VERTEX}
				}, {
				  {0, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(ToonVertex, pos),
				         sizeof(glm::vec3), POSITION},
				  {0, 1, VK_FORMAT_R32G32_SFLOAT, offsetof(ToonVertex, UV),
				         sizeof(glm::vec2), UV},
				  {0, 2, VK_FORMAT_R32G32B32_SFLOAT, offsetof(ToonVertex, norm),
				         sizeof(glm::vec3), NORMAL}
				});

		VDSkyBox.init(this, {
				  {0, sizeof(SkyBoxVertex), VK_VERTEX_INPUT_RATE_VERTEX}
				}, {
				  {0, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(SkyBoxVertex, pos),
				         sizeof(glm::vec3), POSITION}
				});

		// Pipelines [Shader couples]
    PToon.init(this, &VDToon,  "shaders/Vert.spv",    "shaders/ToonFrag.spv",  {&DSLToon});
    PBW.init(this, &VDToon, "shaders/Vert.spv",      "shaders/BWFrag.spv", {&DSLBW});
		PSkyBox.init(this, &VDSkyBox, "shaders/SkyBoxVert.spv", "shaders/SkyBoxFrag.spv", {&DSLSkyBox});
		PSkyBox.setAdvancedFeatures(VK_COMPARE_OP_LESS_OR_EQUAL, VK_POLYGON_MODE_FILL, VK_CULL_MODE_BACK_BIT, false);

		// Create Models
    MCar.init(this, &VDToon, "models/Car.mgcg", MGCG);
    MMike.init(this, &VDToon, "models/Mike.obj", OBJ);
    MSkyBox.init(this, &VDToon, "models/SkyBox.obj", OBJ);
		
		// Create the textures
    
		TGeneric.init(this, "textures/Textures.png");
		TMike.init(this, "textures/T_Zebra.png");
		TSkyBox.init(this, "textures/T_SkyBox.jpg");


		// Descriptor pool sizes
		// WARNING!!!!!!!!
		// Must be set before initializing the text and the scene
		DPSZs.uniformBlocksInPool = 10;
		DPSZs.texturesInPool = 10;
		DPSZs.setsInPool = 10;

	
std::cout << "Initializing text\n";
		txt.init(this, &outText);

		std::cout << "Initialization completed!\n";
		std::cout << "Uniform Blocks in the Pool  : " << DPSZs.uniformBlocksInPool << "\n";
		std::cout << "Textures in the Pool        : " << DPSZs.texturesInPool << "\n";
		std::cout << "Descriptor Sets in the Pool : " << DPSZs.setsInPool << "\n";
	}
	
	// Here you create your pipelines and Descriptor Sets!
	void pipelinesAndDescriptorSetsInit() {
		// This creates a new pipeline (with the current surface), using its shaders
		PBW.create();
    PToon.create();
    PSkyBox.create();

		// Here you define the data set
    DSCar.init(this, &DSLToon, {&TGeneric});
		DSMike.init(this, &DSLBW, {&TMike});

		DSSkyBox.init(this, &DSLSkyBox, {&TSkyBox});
		DSGlobal.init(this, &DSLGlobal, {});

		txt.pipelinesAndDescriptorSetsInit();		
	}

	// Here you destroy your pipelines and Descriptor Sets!
	// All the object classes defined in Starter.hpp have a method .cleanup() for this purpose
	void pipelinesAndDescriptorSetsCleanup() {
		// Cleanup pipelines
		PToon.cleanup();
		PSkyBox.cleanup();
		PBW.cleanup();

		DSCar.cleanup();
		DSMike.cleanup();
    DSSkyBox.cleanup();
		DSGlobal.cleanup();

		txt.pipelinesAndDescriptorSetsCleanup();
	}

	// Here you destroy all the Models, Texture and Desc. Set Layouts you created!
	// All the object classes defined in Starter.hpp have a method .cleanup() for this purpose
	// You also have to destroy the pipelines: since they need to be rebuilt, they have two different
	// methods: .cleanup() recreates them, while .destroy() delete them completely
	void localCleanup() {	
		TGeneric.cleanup();
    TMike.cleanup();
    TSkyBox.cleanup();

    MCar.cleanup();
    MMike.cleanup();
		MSkyBox.cleanup();

		// Cleanup descriptor set layouts
		DSLToon.cleanup();
		DSLBW.cleanup();
    DSLSkyBox.cleanup();
		DSLGlobal.cleanup();
		
		// Destroies the pipelines
		PToon.destroy();
		PBW.destroy();
    PSkyBox.destroy();

		txt.localCleanup();		
	}
	
	// Here it is the creation of the command buffer:
	// You send to the GPU all the objects you want to draw,
	// with their buffers and textures
	
	void populateCommandBuffer(VkCommandBuffer commandBuffer, int currentImage) {
    std::cout << "gay";
		// binds the pipeline
		PToon.bind(commandBuffer);
		MCar.bind(commandBuffer);
		DSGlobal.bind(commandBuffer, PToon, 0, currentImage);	// The Global Descriptor Set (Set 0)
		DSCar.bind(commandBuffer, PToon, 1, currentImage);	// The Material and Position Descriptor Set (Set 1)
                                                        
		vkCmdDrawIndexed(commandBuffer,
				static_cast<uint32_t>(MCar.indices.size()), 1, 0, 0, 0);	
                                                        
    PBW.bind(commandBuffer);
		MMike.bind(commandBuffer);
		DSGlobal.bind(commandBuffer, PBW, 0, currentImage);	// The Global Descriptor Set (Set 0)
		DSMike.bind(commandBuffer, PBW, 1, currentImage);	// The Material and Position Descriptor Set (Set 1)

		vkCmdDrawIndexed(commandBuffer,
				static_cast<uint32_t>(MMike.indices.size()), 1, 0, 0, 0);

		PSkyBox.bind(commandBuffer);
    MSkyBox.bind(commandBuffer);
		DSMike.bind(commandBuffer, PBW, 2, currentImage);

		vkCmdDrawIndexed(commandBuffer,
				static_cast<uint32_t>(MSkyBox.indices.size()), 1, 0, 0, 0);	

		txt.populateCommandBuffer(commandBuffer, currentImage, currScene);
	}

	// Here is where you update the uniforms.
	// Very likely this will be where you will be writing the logic of your application.
	void updateUniformBuffer(uint32_t currentImage) {
		static bool debounce = false;
		static int curDebounce = 0;

		float deltaT;
		glm::vec3 m = glm::vec3(0.0f), r = glm::vec3(0.0f);
		bool fire = false;
		getSixAxis(deltaT, m, r, fire);
		
		static float autoTime = true;
		static float cTime = 0.0;
		const float turnTime = 36.0f;
		const float angTurnTimeFact = 2.0f * M_PI / turnTime;
		
		if(autoTime) {
			cTime = cTime + deltaT;
			cTime = (cTime > turnTime) ? (cTime - turnTime) : cTime;
		}
		cTime += r.z * angTurnTimeFact * 4.0;
		
		const float ROT_SPEED = glm::radians(120.0f);
		const float MOVE_SPEED = 2.0f;
		
		CamAlpha = CamAlpha - ROT_SPEED * deltaT * r.y;
		CamBeta  = CamBeta  - ROT_SPEED * deltaT * r.x;
		CamBeta  =  CamBeta < glm::radians(-90.0f) ? glm::radians(-90.0f) :
				   (CamBeta > glm::radians( 90.0f) ? glm::radians( 90.0f) : CamBeta);

		glm::vec3 ux = glm::rotate(glm::mat4(1.0f), CamAlpha, glm::vec3(0,1,0)) * glm::vec4(1,0,0,1);
		glm::vec3 uz = glm::rotate(glm::mat4(1.0f), CamAlpha, glm::vec3(0,1,0)) * glm::vec4(0,0,1,1);
		CamPos = CamPos + MOVE_SPEED * m.x * ux * deltaT;
		CamPos = CamPos + MOVE_SPEED * m.y * glm::vec3(0,1,0) * deltaT;
		CamPos = CamPos + MOVE_SPEED * m.z * uz * deltaT;
		
		static float subpassTimer = 0.0;

		if(glfwGetKey(window, GLFW_KEY_SPACE)) {
			if(!debounce) {
				debounce = true;
				curDebounce = GLFW_KEY_SPACE;
				if(currScene != 1) {
					currScene = (currScene+1) % outText.size();

				}
				if(currScene == 1) {
					if(subpass >= 4) {
						currScene = 0;
					}
				}
				std::cout << "Scene : " << currScene << "\n";
				
				RebuildPipeline();
			}
		} else {
			if((curDebounce == GLFW_KEY_SPACE) && debounce) {
				debounce = false;
				curDebounce = 0;
			}
		}

		// Standard procedure to quit when the ESC key is pressed
		if(glfwGetKey(window, GLFW_KEY_ESCAPE)) {
			glfwSetWindowShouldClose(window, GL_TRUE);
		}


		if(glfwGetKey(window, GLFW_KEY_V)) {
			if(!debounce) {
				debounce = true;
				curDebounce = GLFW_KEY_V;

				printVec3("CamPos  ", CamPos);				
				std::cout << "CamAlpha = " << CamAlpha << ";\n";
				std::cout << "CamBeta  = " << CamBeta  << ";\n";
				std::cout << "cTime    = " << cTime    << ";\n";
			}
		} else {
			if((curDebounce == GLFW_KEY_V) && debounce) {
				debounce = false;
				curDebounce = 0;
			}
		}

	
/*		if(currScene == 1) {
			switch(subpass) {
			  case 0:
CamPos   = glm::vec3(0.0644703, 6.442, 8.83251);
CamAlpha = 0;
CamBeta  = -0.4165;
cTime    = 2.40939;
autoTime = false;
				break;
			  case 1:
CamPos   = glm::vec3(-1.21796, 6.82323, 5.58497);
CamAlpha = 0.284362;
CamBeta  = -0.58455;
cTime    = 23.3533;
				break;
			  case 2:
CamPos   = glm::vec3(0.921455, 3.97743, 0.855181);
CamAlpha = -1.16426;
CamBeta  = -0.388393;
cTime    = 36.6178;
				break;
			  case 3:
 CamPos   = glm::vec3(5.59839, 4.04786, 2.59767);
CamAlpha = 1.01073;
CamBeta  = -0.213902;
cTime    = 15.6739;
				break;
			}
		}
		
		if(currScene == 1) {
			subpassTimer += deltaT;
			if(subpassTimer > 4.0f) {
				subpassTimer = 0.0f;
				subpass++;
				std::cout << "Scene : " << currScene << " subpass: " << subpass << "\n";
				char buf[20];
				sprintf(buf, "A08_%d.png", subpass);
				saveScreenshot(buf, currentImage);
				if(subpass == 4) {
CamPos   = glm::vec3(0, 1.5, 7);
CamAlpha = 0;
CamBeta  = 0;
autoTime = true;
					currScene = 0;
					std::cout << "Scene : " << currScene << "\n";
					RebuildPipeline();
				}
			}
		}*/


		// Here is where you actually update your uniforms
	}
};

// This is the main: probably you do not need to touch this!
int main() {
    Application app;

    try {
        app.run();
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
