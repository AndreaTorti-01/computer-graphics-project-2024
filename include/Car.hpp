#pragma once

#include "Entity.hpp"
#include "Bullet.hpp"
#include "Mike.hpp"
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
	glm::mat4 position4;

	// weapons
	float shootCooldown;
	float timeSinceLastShot;
	std::array<Bullet, MAX_BULLET_INSTANCES> bullets;

	// Car status
	float carSpeed;
	float currentSteeringAngle;

	int health;

public:
	Car();
	float getSpeed();
	void damage();
	glm::mat4 getPosition4();
	void setSinceLastShot(float time);
	float getSinceLastShot();
	std::array<Bullet, MAX_BULLET_INSTANCES> getBullets();
	void update(float deltaT, glm::vec3& controls);
	void check_collisions(std::array<Mike, MAX_MIKE_INSTANCES>& mikes);
};

