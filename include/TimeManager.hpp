#pragma once
#include <chrono>
#include "Car.hpp"
#include "Mike.hpp"

class TimeManager {
private:
	std::chrono::steady_clock::time_point lastFrameTime;
	float deltaTime;
	float mikeSpawnTimer;
	float mikeSpawnCooldown;
	std::chrono::steady_clock::time_point lastAccelleration;

public:
	TimeManager();
	void update();
	void updateTimers(Car car, std::array<Mike, MAX_MIKE_INSTANCES> &mikes);
	float getDeltaTime() const;
};