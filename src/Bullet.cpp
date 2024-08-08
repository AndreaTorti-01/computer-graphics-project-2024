#include "Bullet.hpp"

Bullet::Bullet()
{
	position = glm::vec3(0.0f, -2.0f, 0.0f);
	rotation = 0.0f;
	isAboveFloor = false;

	bulletSpeed = 14.0f;
	flyTime = 0.0f;
	maxFlyTime = 2.0f;
}


void Bullet::update(float deltaTime)
{
	// if it is above the floor then update its position
	if (isAboveFloor)
	{
		position += direction * bulletSpeed * deltaTime;
		flyTime += deltaTime;

		// if it has been flying for too long then reset it
		if (flyTime >= maxFlyTime)
		{
		    explode();
		}
	}
}


void Bullet::shoot(const glm::vec3& position, glm::vec3 direction)
{
	this->position = position;
	direction.y = 0.0f;
	this->direction = glm::normalize(direction);
	isAboveFloor = true;
	flyTime = 0.0f;
}

void Bullet::explode()
{
	isAboveFloor = false;
	position = glm::vec3(0.0f, -2.0f, 0.0f);
}
