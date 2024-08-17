#include "TimeManager.hpp"
#include "Car.hpp"
#include "Mike.hpp"
#include <array>

TimeManager::TimeManager()
{
    lastFrameTime = std::chrono::steady_clock::now();
    deltaTime = 0.0f;
    mikeSpawnTimer = 0.0f;
    mikeSpawnCooldown = 2.0f;
    upgradeSpawnTimer = 0.0f;
    upgradeSpawnCooldown = 2.0f;
    startOfProgram = std::chrono::steady_clock::now();
    lastAccelleration = std::chrono::steady_clock::now();
}

void TimeManager::update()
{
    auto currentTime = std::chrono::steady_clock::now();
    deltaTime = std::chrono::duration<float>(currentTime - lastFrameTime).count();
    lastFrameTime = currentTime;
}

float TimeManager::getPassedTime()
{
    auto currentTime = std::chrono::steady_clock::now();
    return std::chrono::duration<float>(currentTime - startOfProgram).count();
}

float TimeManager::getDeltaTime() const
{
    return deltaTime;
}

void TimeManager::updateTimers(Car car, std::array<Mike, MAX_MIKE_INSTANCES> &mikes, std::array<Upgrade, MAX_UPGRADE_INSTANCES> &upgrades)
{
    bool accelerate = false;
    if (std::chrono::duration<float>(lastFrameTime - lastAccelleration).count() >= 10.0f)
    {
        lastAccelleration = lastFrameTime;
        accelerate = true;
    }

    for (auto &mike : mikes)
    {
        if (accelerate)
            mike.multiplySpeed(1.2f);
        if (!mike.getIsAboveFloor())
        {
            mikeSpawnTimer += deltaTime;
            if (mikeSpawnTimer >= mikeSpawnCooldown)
            {
                mikeSpawnTimer = 0.0f;
                mike.spawn(car.getPosition());
            }
            break;
        }
    }
    for (auto &up : upgrades)
    {
        if (!up.getIsAboveFloor())
        {
            upgradeSpawnTimer += deltaTime;
            if (upgradeSpawnTimer >= upgradeSpawnCooldown)
            {
                upgradeSpawnTimer = 0.0f;
                up.spawn(car.getPosition());
            }
            break;
        }
    }
}