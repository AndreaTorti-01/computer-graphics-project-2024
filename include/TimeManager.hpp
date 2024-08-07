#pragma once
#include <chrono>

class TimeManager {
private:
	std::chrono::steady_clock::time_point lastFrameTime;
	float deltaTime;

public:
	TimeManager();
	void update();
	float getDeltaTime() const;
};