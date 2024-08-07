#pragma once
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEFAULT_ALIGNED_GENTYPES
#define GLM_FORCE_DEPTH_ZERO_TO_ONE

#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/quaternion.hpp>
#include <gtx/transform2.hpp>

#include <chrono>
#include <vector>

void update_bullet_positions(glm::vec3 carPos, std::vector<BulletInstance> &bullets, glm::vec3 direction, float &fireTimer, float deltaT)
{
    // Update Bullet instances


    fireTimer += deltaT;

    if (fireTimer >= 0.4f)
    {
        fireTimer = 0.0;
        for (auto &bullet : bullets)
        {
            if (!bullet.isAboveFloor)
            {
                bullet.position = carPos;
                bullet.isAboveFloor = true;
                bullet.dir = direction;
                bullet.dir.y = 0.0f;
                bullet.lifetime = 0.0f;
                break;
            }
        }
    }
    for (auto &bullet : bullets)
    {
        if (bullet.isAboveFloor)
        {
            bullet.position += bullet.dir * 5.0f * deltaT;
            bullet.lifetime += deltaT;
            if (bullet.lifetime >= 2.0f)
            {
                bullet.isAboveFloor = false;
                bullet.position.y = -2.0f;
            }
        }
    }

}