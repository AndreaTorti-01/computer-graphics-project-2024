#pragma once

#include <glm_with_defines.hpp>
#include <Utils.hpp>

class Entity {
protected:
	glm::vec3 position;
	float rotation;
	bool isAboveFloor;

public:
	/**
  * @brief Update the entity's state.
  *
  * This method should be implemented by derived classes to update the entity's state based on the elapsed time.
  *
  * @param deltaTime The time elapsed since the last update.
  */
	virtual void update(float deltaTime) = 0;

	glm::vec3 getPosition() const;
	void setPosition(const glm::vec3& pos);
	float getRotation() const;
	void setRotation(float rot);
	bool getIsAboveFloor() const;
	void setIsAboveFloor(bool above);
};