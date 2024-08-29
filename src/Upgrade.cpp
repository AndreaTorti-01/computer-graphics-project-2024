#include "Upgrade.hpp"

Upgrade::Upgrade(){
	minRadius = 10.0f;
	maxRadius = 20.0f;
	rotSpeed = 0.5f;
	rng = std::mt19937(rd());
	position = glm::vec3(0.0f, -2.0f, 0.0f);
	position4 = glm::translate(glm::mat4(1.0f), position);
	rotation = 0.0f;
	isAboveFloor = false;
}

glm::mat4 Upgrade::getPosition4(){
	return position4;
}

void Upgrade::reset(){
	isAboveFloor = false;
	position = glm::vec3(0.0f, -2.0f, 0.0f);
	position4 = glm::translate(glm::mat4(1.0f), position);
	rotation = 0.0f;
}

void Upgrade::update(float deltaT){
	if (isAboveFloor)
	{
		rotation += rotSpeed * deltaT;
		position4 = glm::rotate(position4, rotSpeed * deltaT, glm::vec3(0.0f, 1.0f, 0.0f));
	}
}

void Upgrade::spawn(glm::vec3 pos){
	position = generateRandomPosition(pos);
	position.y = 0.0f;
	position4 = glm::translate(glm::mat4(1.0f), position);
	position4 = glm::scale(position4, glm::vec3(1.5f));
	rotation = 0.0f;
	isAboveFloor = true;
}

int Upgrade::getUpgrade(){
	if(isAboveFloor){
		reset();
		std::uniform_real_distribution<float> type(0,1); // 0 bullet cooldown reduction, 1 bullet speed increase
		return type(rng);
	}
	return -1;
}
    
glm::vec3 Upgrade::generateRandomPosition(const glm::vec3 &carPosition)
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

