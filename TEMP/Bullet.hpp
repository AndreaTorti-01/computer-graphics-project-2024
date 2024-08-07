#pragma once

#include "Entity.hpp"

class Bullet :
	public Entity
{
private:
	float bulletSpeed;
	float flyTime;
	float maxFlyTime;

public:
	Bullet();

	void update(float deltaTime);
	void shoot(const glm::vec3& position, float rotation);
	void explode();
};
