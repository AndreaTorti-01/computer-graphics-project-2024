#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEFAULT_ALIGNED_GENTYPES
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/transform2.hpp>

#include <chrono>
#include <vector>
#include <random> // Include random number generation library

// Structure to represent an instance of Mike
struct MikeInstance {
	glm::vec3 position; // Position of Mike instance
	float rotation;     // Rotation of Mike instance
	bool isAboveFloor;  // Flag to indicate if Mike is above or below the floor
};

// Function to generate a random position around the car within the floor boundaries
glm::vec3 generateRandomPosition(const glm::vec3& carPosition, const float minRadius, const float maxRadius, std::mt19937& rng, const float floorDiam) {
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

void update_mike_positions(const glm::vec3& carPosition, std::vector<MikeInstance>& mikes, float& mikeSpawnTimer, const float deltaT, std::mt19937& rng, const float minRadius, const float maxRadius, const float floorDiam) {
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