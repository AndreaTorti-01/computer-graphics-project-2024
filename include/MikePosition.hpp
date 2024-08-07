#pragma once

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEFAULT_ALIGNED_GENTYPES
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/quaternion.hpp>
#include <gtx/transform2.hpp>

#include <chrono>
#include <vector>
#include <random> // Include random number generation library

// Function to generate a random position around the car within the floor boundaries
glm::vec3 generateRandomPosition(const glm::vec3& carPosition, const float minRadius, const float maxRadius, , const float floorDiam) {
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

    for (auto &mike : mikes)
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

void check_collisions_MB(std::vector<MikeInstance> &mikes, std::vector<BulletInstance> &bullets)
{
	for (auto &mike : mikes)
	{
		for (auto &bullet : bullets)
		{
			if (mike.isAboveFloor && bullet.isAboveFloor)
			{
				if (glm::distance(mike.position, bullet.position) <= 0.5)
				{
					mike.isDamaged = true;
					mike.damageTimer = 0.0;
					bullet.isAboveFloor = false;
					bullet.lifetime = 0.0;
					bullet.position.y = -2.0;
				}
			}
		}
	}
}

bool check_collisions_MC(std::vector<MikeInstance> &mikes, glm::vec3 &CarPosition)
{
	for (auto &mike : mikes)
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