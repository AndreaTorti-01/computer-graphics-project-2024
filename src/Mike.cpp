#include "Mike.hpp"


glm::vec3 Mike::generateRandomPosition(const glm::vec3 &carPosition)
{
	std::uniform_real_distribution<float> distRadius(minRadius, maxRadius);
	std::uniform_real_distribution<float> distAngle(0.0f, 2.0f * glm::pi<float>());
	const float floorDiamAdj = (FLOOR_DIAM - 1.0f) / 2.0f;

	float radius, angle;
	glm::vec3 offset;
	glm::vec3 result;
	do
	{
		radius = distRadius(rng);
		angle = distAngle(rng);
		offset = glm::vec3(radius * cos(angle), 0.0f, radius * sin(angle));
		result = carPosition + offset;
	} while (-floorDiamAdj > result.x || result.x > floorDiamAdj || -floorDiamAdj > result.z || result.z > floorDiamAdj);

	return result;
}

Mike::Mike()
{
	minRadius = 3.0f;
	maxRadius = 7.0f;
	rng = std::mt19937(rd());
	isAboveFloor = false;
	isDamaged = false;
	damageTimer = 0.0;
	position = glm::vec3(0.0f, -2.0f, 0.0f);
	rotation = 0.0f;
	speed = 2.0f;
}

void Mike::setDamaged(bool damaged) { this->isDamaged = damaged; }
bool Mike::getDamaged() { return  this->isDamaged; }
void Mike::setDamageTimer(float timer) { damageTimer = timer; }
void Mike::multiplySpeed(float multiplier) { if(speed * multiplier <= 9.0f ) speed *= multiplier; }
void Mike::reset()
{
	isAboveFloor = false;
	isDamaged = false;
	damageTimer = 0.0;
	position = glm::vec3(0.0f, -2.0f, 0.0f);
	rotation = 0.0f;
}

void Mike::spawn(glm::vec3 pos)
{
	position = generateRandomPosition(pos);
	position.y = 0.0f;
	isAboveFloor = true;
}

void Mike::update(float deltaT, const glm::vec3 &carPosition)
{
	// if it is above the floor then update its position to move towards the player
	if (isAboveFloor)
	{
		if (!isDamaged)
		{
			glm::vec3 dirToPlayer = glm::normalize(carPosition - position);
			dirToPlayer.y = 0.0f;
			position += dirToPlayer * speed * deltaT;
			rotation = std::atan2(dirToPlayer.x, dirToPlayer.z);
		}
		if (isDamaged)
			damageTimer += deltaT;
		if (damageTimer >= 0.5)
		{
			reset();
		}
	}
}
