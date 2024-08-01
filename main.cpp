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
	glm::vec2 UV;
	glm::vec3 norm;
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
  glm::mat4 ViewMatrix;
	
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
    PToon.init(this, &VDToon,  "shaders/Vert.spv",    "shaders/ToonFrag.spv",  {&DSLGlobal, &DSLToon});
    PBW.init(this, &VDToon, "shaders/Vert.spv",      "shaders/BWFrag.spv", {&DSLGlobal, &DSLBW});
		PSkyBox.init(this, &VDSkyBox, "shaders/SkyBoxVert.spv", "shaders/SkyBoxFrag.spv", {&DSLSkyBox});
		PSkyBox.setAdvancedFeatures(VK_COMPARE_OP_LESS_OR_EQUAL, VK_POLYGON_MODE_FILL, VK_CULL_MODE_BACK_BIT, false);

		// Create Models
    MCar.init(this, &VDToon, "models/Car.mgcg", MGCG);

    if (MCar.indices.empty()){
      std::cout << "car not loaded";
      exit(0);
    }

    MMike.init(this, &VDToon, "models/Mike.obj", OBJ);
    MSkyBox.init(this, &VDSkyBox, "models/SkyBox.obj", OBJ);
		
		// Create the textures
    
		TGeneric.init(this, "textures/Textures.png");
		TMike.init(this, "textures/T_Zebra.png");
		TSkyBox.init(this, "textures/Textures.png");



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

		ViewMatrix = glm::translate(glm::mat4(1), -CamPos);
	}
	
	// Here you create your pipelines and Descriptor Sets!
	void pipelinesAndDescriptorSetsInit() {
    
    std::cout << "pipeline and descriptors init started \n";
		// This creates a new pipeline (with the current surface), using its shaders
		PBW.create();
    PToon.create();
    PSkyBox.create();

		// Here you define the data set
    DSCar.init(this, &DSLToon, {&TGeneric});
		DSMike.init(this, &DSLBW, {&TMike});

		DSSkyBox.init(this, &DSLSkyBox, {&TSkyBox, &TSkyBox});
		DSGlobal.init(this, &DSLGlobal, {});

		txt.pipelinesAndDescriptorSetsInit();		
    std::cout << "pipeline and descriptors init stopped \n";
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
    

    std::cout << "Car started \n";

		// binds the pipeline
		PToon.bind(commandBuffer);
		MCar.bind(commandBuffer);
		DSGlobal.bind(commandBuffer, PToon, 0, currentImage);	// The Global Descriptor Set (Set 0)
		DSCar.bind(commandBuffer, PToon, 1, currentImage);	// The Material and Position Descriptor Set (Set 1)
                                                        
		vkCmdDrawIndexed(commandBuffer,
				static_cast<uint32_t>(MCar.indices.size()), 1, 0, 0, 0);	
                                                        
    std::cout << "Mike started \n";

    PBW.bind(commandBuffer);
		MMike.bind(commandBuffer);
		DSGlobal.bind(commandBuffer, PBW, 0, currentImage);	// The Global Descriptor Set (Set 0)
		DSMike.bind(commandBuffer, PBW, 1, currentImage);	// The Material and Position Descriptor Set (Set 1)

		vkCmdDrawIndexed(commandBuffer,
				static_cast<uint32_t>(MMike.indices.size()), 1, 0, 0, 0);

    std::cout << "SkyBox started \n";

		PSkyBox.bind(commandBuffer);
    MSkyBox.bind(commandBuffer);
		DSSkyBox.bind(commandBuffer, PSkyBox, 0, currentImage);

		vkCmdDrawIndexed(commandBuffer,
				static_cast<uint32_t>(MSkyBox.indices.size()), 1, 0, 0, 0);	

		txt.populateCommandBuffer(commandBuffer, currentImage, currScene);
	}

	// Here is where you update the uniforms.
	// Very likely this will be where you will be writing the logic of your application.
	void updateUniformBuffer(uint32_t currentImage) {

    // Update MVP matrices
    //glm::mat4 view = glm::lookAt(CamPos, CamPos + getCameraFront(), glm::vec3(0.0f, 1.0f, 0.0f));
    glm::mat4 proj = glm::perspective(glm::radians(45.0f), Ar, 0.1f, 100.0f);
    proj[1][1] *= -1;  // Flip Y coordinate for Vulkan

		glm::mat4 view =  proj * ViewMatrix;
    // Update Car uniforms
    ToonUniformBufferObject uboToon{};
    uboToon.mvpMat = proj * view * glm::mat4(1.0f);  // Adjust model matrix as needed
    uboToon.mMat = glm::mat4(1.0f);  // Adjust as needed
    uboToon.nMat = glm::transpose(glm::inverse(uboToon.mMat));
    DSCar.map(currentImage, &uboToon, 0);

    // Update Mike uniforms
    DSMike.map(currentImage, &uboToon, 0);

    // Update Skybox uniforms
    SkyBoxUniformBufferObject uboSky{};
    uboSky.mvpMat = proj * glm::mat4(glm::mat3(view));  // Remove translation from view matrix
    DSSkyBox.map(currentImage, &uboSky, 0);

    // Update global uniforms
    GlobalUniformBufferObject uboGlobal{};
    uboGlobal.lightDir = glm::normalize(glm::vec3(1.0f, 1.0f, 1.0f));
    uboGlobal.lightColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
    uboGlobal.eyePos = CamPos;
    DSGlobal.map(currentImage, &uboGlobal, 0);
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
