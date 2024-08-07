#pragma once

#include "Entity.hpp"
#include <random>
#include <glm_with_defines.hpp>
#include <Utils.hpp>

class Mike :
	public Entity
{
private:
	float minRadius;
	float maxRadius;
	std::random_device rd;
	std::mt19937 rng;
	bool isDamaged;
	float damageTimer;

	glm::vec3 generateRandomPosition(const glm::vec3& carPosition);

public:
	Mike();
	void update(const glm::vec3& carPosition, float deltaT);
};

