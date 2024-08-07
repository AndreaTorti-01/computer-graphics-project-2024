#include "TimeManager.hpp"

TimeManager::TimeManager() : lastFrameTime(std::chrono::steady_clock::now()), deltaTime(0.0f) {}

void TimeManager::update() {
    auto currentTime = std::chrono::steady_clock::now();
    deltaTime = std::chrono::duration<float>(currentTime - lastFrameTime).count();
    lastFrameTime = currentTime;
}

float TimeManager::getDeltaTime() const {
    return deltaTime;
}
