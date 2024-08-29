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
	float upgradeSpawnTimer;
	float upgradeSpawnCooldown;
	std::chrono::steady_clock::time_point startOfProgram;
	std::chrono::steady_clock::time_point lastAccelleration;
	std::chrono::steady_clock::time_point lastViewChange;

public:
	TimeManager();
	void update();
	float getPassedTime();
	bool canChangeView();
	void updateTimers(Car car, std::array<Mike, MAX_MIKE_INSTANCES> &mikes, std::array<Upgrade, MAX_UPGRADE_INSTANCES> &upgrades);
	float getDeltaTime() const;
};