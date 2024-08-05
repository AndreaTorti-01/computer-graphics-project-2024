#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEFAULT_ALIGNED_GENTYPES
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/transform2.hpp>

#include <chrono>
#include <vector>


// Structure to represent an instance of Mike
struct MikeInstance {
	glm::vec3 position; // Position of Mike instance
	float rotation;     // Rotation of Mike instance
	bool isAboveFloor;  // Flag to indicate if Mike is above or below the floor
};


void update_mike_positions(glm::vec3 &carPosition, std::vector<MikeInstance> &mikes, float &mikeSpawnTimer, float deltaT){
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
}