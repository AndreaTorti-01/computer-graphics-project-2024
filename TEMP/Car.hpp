#pragma once

#include "Entity.hpp"
#include "Bullet.hpp"
#include <array>

class Car :
	public Entity
{
private:
	// Car properties
	float maxSpeed;
	float acceleration;
	float deceleration;
	float maxSteeringAngle;
	float steeringSpeed;
	float wheelbase;

	// weapons
	float shootCooldown;
	float timeSinceLastShot;
	std::array<Bullet, MAX_BULLET_INSTANCES> bullets;

	// Car status
	float carSpeed;
	float currentSteeringAngle;

public:
	Car();

	void update(float deltaTime, const glm::vec3& controls);
};

