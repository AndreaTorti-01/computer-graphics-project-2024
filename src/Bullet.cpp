#include "Bullet.hpp"

Bullet::Bullet()
{
	position = glm::vec3(0.0f, -2.0f, 0.0f);
	position4 = glm::translate(glm::mat4(0.0f), position);
	rotation = 0.0f;
	isAboveFloor = false;

	bulletSpeed = 14.0f;
	flyTime = 0.0f;
	maxFlyTime = 1.0f;
}

glm::mat4 Bullet::getPosition4() { return position4; }


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

void Bullet::increaseSpeed()
{
	bulletSpeed = glm::clamp(bulletSpeed + 2.0f, 14.0f, 28.0f);
}


void Bullet::shoot(const glm::vec3& position, float rotation, glm::vec3 direction)
{
	this->position = position;
	direction.y = 0.0f;
	this->rotation = -rotation;
	this->direction = glm::normalize(direction);
	isAboveFloor = true;
	flyTime = 0.0f;
}

void Bullet::explode()
{
	isAboveFloor = false;
	position = glm::vec3(0.0f, -2.0f, 0.0f);
}
