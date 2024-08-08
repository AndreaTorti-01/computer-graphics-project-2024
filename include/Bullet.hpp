#pragma once

#include "Entity.hpp"

class Bullet :
	public Entity
{
private:
	float bulletSpeed;
	float flyTime;
	float maxFlyTime;
	glm::vec3 direction;

public:
	Bullet();

	void update(float deltaTime);
	void shoot(const glm::vec3& position, glm::vec3 direction);
	void explode();
};
