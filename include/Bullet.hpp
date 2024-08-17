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
	glm::mat4 position4;

public:
	Bullet();

	void update(float deltaTime);
	void shoot(const glm::vec3& position, float rotation, glm::vec3 direction);
	glm::mat4 getPosition4();
	void explode();
	void reduceCooldown();
	void increaseSpeed();
};
