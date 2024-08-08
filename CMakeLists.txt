cmake_minimum_required(VERSION 3.10)

# Project name and version
project(Computer-graphics-project VERSION 1.0)

# Specify the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Glob all source and header files
file(GLOB SOURCES "src/*.cpp")
file(GLOB HEADERS "include/*.h")

# Add the include directory for headers
include_directories(include)

# Add GLM library
find_package(glm REQUIRED)

if(glm_FOUND)
    message(STATUS "Found GLM: ${glm_INCLUDE_DIR}")
else()
    message(FATAL_ERROR "Could not find GLM library")
endif()

# Add Vulkan library
find_package(Vulkan REQUIRED)

if(Vulkan_FOUND)
    message(STATUS "Found Vulkan: ${Vulkan_INCLUDE_DIRS}")
else()
    message(FATAL_ERROR "Could not find Vulkan library")
endif()

# Add GLFW library
find_package(glfw3 3.3 REQUIRED)

if(glfw3_FOUND)
    message(STATUS "Found GLFW: ${glfw3_INCLUDE_DIRS}")
else()
    message(FATAL_ERROR "Could not find GLFW library")
endif()

# Include directories for additional libraries
include_directories(${CMAKE_SOURCE_DIR}/libraries)

# Add executable target
add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})

# Link libraries
target_link_libraries(${PROJECT_NAME} PRIVATE glm Vulkan::Vulkan glfw)

# Set include directories
target_include_directories(${PROJECT_NAME} PRIVATE
    ${glm_INCLUDE_DIR}
    ${Vulkan_INCLUDE_DIRS}
    ${glfw3_INCLUDE_DIRS}
    ${CMAKE_SOURCE_DIR}/libraries
)

# Optionally, specify additional directories to search for libraries
link_directories(${CMAKE_SOURCE_DIR}/libraries)

# Compile shaders with glslc
file(GLOB SHADERS "shaders/*.vert" "shaders/*.frag")

foreach(SHADER ${SHADERS})
    get_filename_component(FILE_NAME_WE ${SHADER} NAME_WE)
    string(REPLACE "Shader" "" BASE_NAME ${FILE_NAME_WE})
    
    if("${SHADER}" MATCHES ".vert$")
        set(OUTPUT_FILE ${CMAKE_SOURCE_DIR}/shaders/${BASE_NAME}Vert.spv)
    elseif("${SHADER}" MATCHES ".frag$")
        set(OUTPUT_FILE ${CMAKE_SOURCE_DIR}/shaders/${BASE_NAME}Frag.spv)
    endif()

    add_custom_command(
        OUTPUT ${OUTPUT_FILE}
        COMMAND glslc ${SHADER} -o ${OUTPUT_FILE}
        DEPENDS ${SHADER}
        COMMENT "Compiling ${FILE_NAME_WE} to ${OUTPUT_FILE}"
    )
    list(APPEND SPIRV_SHADERS ${OUTPUT_FILE})
endforeach()

# Ensure the shaders are built before the main executable
add_custom_target(ShadersTarget DEPENDS ${SPIRV_SHADERS})
add_dependencies(${PROJECT_NAME} ShadersTarget)

# Add custom commands if needed
# For example, you can add post-build commands, copy files, etc.
