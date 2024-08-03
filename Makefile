SRC_NAME = main
ADD_FILES =


CFLAGS = -std=c++17 -Iheaders 
LDFLAGS = -lglfw -lvulkan -ldl -lpthread -lX11  -lXrandr

$(SRC_NAME): $(SRC_NAME).cpp $(ADD_FILES) shaders
	g++ $(CFLAGS) -o $(SRC_NAME) $(SRC_NAME).cpp $(LDFLAGS)

shaders:
	/usr/bin/glslc shaders/Shader.vert -o shaders/Vert.spv
	/usr/bin/glslc shaders/NormalMapShader.vert -o shaders/NormalMapVert.spv
	/usr/bin/glslc shaders/ToonShader.frag -o shaders/ToonFrag.spv
	/usr/bin/glslc shaders/ToonNormalShader.frag -o shaders/ToonNormalFrag.spv
	/usr/bin/glslc shaders/TextShader.vert -o shaders/TextVert.spv
	/usr/bin/glslc shaders/TextShader.frag -o shaders/TextFrag.spv
	/usr/bin/glslc shaders/BWShader.frag -o shaders/BWFrag.spv
	/usr/bin/glslc shaders/SkyBoxShader.frag -o shaders/SkyBoxFrag.spv
	/usr/bin/glslc shaders/SkyBoxShader.frag -o shaders/SkyBoxFrag.spv

