#pragma once

#include "Entity.hpp"
#include <random>
#include <glm_with_defines.hpp>
#include <Utils.hpp>

class Upgrade :
	public Entity
{
private:
	float minRadius;
	float maxRadius;
	std::random_device rd;
	std::mt19937 rng;
	float rotSpeed;
    glm::mat4 position4;
	glm::vec3 generateRandomPosition(const glm::vec3& carPosition);
    void reset();

public:
	Upgrade();
	void spawn(glm::vec3 pos);
    void update(float deltaT);
    glm::mat4 getPosition4();
    int getUpgrade();
};

