#pragma once

#include <glm_with_defines.hpp>
#include <Utils.hpp>

class Entity {
protected:
	glm::vec3 position;
	float rotation;
	bool isAboveFloor;

public:

	glm::vec3 getPosition() const;
	void setPosition(const glm::vec3& pos);
	float getRotation() const;
	void setRotation(float rot);
	bool getIsAboveFloor() const;
	void setIsAboveFloor(bool above);
};