#include "Entity.hpp"

glm::vec3 Entity::getPosition() const {
	return position;
}

void Entity::setPosition(const glm::vec3& pos) {
	position = pos;
}

float Entity::getRotation() const {
	return rotation;
}

void Entity::setRotation(float rot) {
	rotation = rot;
}

bool Entity::getIsAboveFloor() const {
	return isAboveFloor;
}

void Entity::setIsAboveFloor(bool above) {
	isAboveFloor = above;
}