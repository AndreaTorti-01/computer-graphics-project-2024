#include "TimeManager.hpp"
#include "Car.hpp"
#include "Mike.hpp"
#include <array>


TimeManager::TimeManager(){
    lastFrameTime = std::chrono::steady_clock::now();
    deltaTime = 0.0f;
    mikeSpawnTimer = 0.0f;
    mikeSpawnCooldown = 2.0f;
}


void TimeManager::update() {
    auto currentTime = std::chrono::steady_clock::now();
    deltaTime = std::chrono::duration<float>(currentTime - lastFrameTime).count();
    lastFrameTime = currentTime;
}

float TimeManager::getDeltaTime() const {
    return deltaTime;
}

void TimeManager::updateTimers(Car car, std::array<Mike, MAX_MIKE_INSTANCES> &mikes){
    for(auto &mike : mikes){
        if(!mike.getIsAboveFloor()){
            mikeSpawnTimer += deltaTime;
            if(mikeSpawnTimer >= mikeSpawnCooldown){
                mikeSpawnTimer = 0.0f;
                mike.spawn(car.getPosition());
            }
            return;
        }
    }
}