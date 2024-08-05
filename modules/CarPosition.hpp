

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEFAULT_ALIGNED_GENTYPES
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/transform2.hpp>

#include <chrono>

void update_car_position(glm::mat4 &CarPos, glm::vec3 &carPosition, float &carSpeed, float &currentSteeringAngle, float &carRotation, glm::vec3 &m, float deltaT)
{
    // Car properties

    const float MAX_SPEED = 8.0f;
    const float ACCELERATION = 5.0f;
    const float DECELERATION = 20.0f;
    const float MAX_STEERING_ANGLE = glm::radians(35.0f);
    const float STEERING_SPEED = glm::radians(180.0f);
    const float WHEELBASE = 2.0f;

    // Update car speed
    if (m.z > 0.1f)
    {
        if (carSpeed < 0)
        {
            // If moving backward and forward button is pressed, decelerate using DECELERATION
            carSpeed = std::min(carSpeed + DECELERATION * deltaT, 0.0f);
        }
        else
        {
            carSpeed = std::min(carSpeed + ACCELERATION * deltaT, MAX_SPEED);
        }
    }
    else if (m.z < -0.1f)
    {
        if (carSpeed > 0)
        {
            // If moving forward and backward button is pressed, decelerate using DECELERATION
            carSpeed = std::max(carSpeed - DECELERATION * deltaT, 0.0f);
        }
        else
        {
            carSpeed = std::max(carSpeed - ACCELERATION * deltaT, -MAX_SPEED);
        }
    }
    else
    {
        // Apply natural deceleration when no input
        carSpeed = carSpeed > 0 ? std::max(carSpeed - ACCELERATION * deltaT, 0.0f) : carSpeed < 0 ? std::min(carSpeed + ACCELERATION * deltaT, 0.0f)
                                                                                                  : 0.0f;
    }

    // Update steering angle incrementally
    float targetSteeringAngle = -MAX_STEERING_ANGLE * m.x;
    float steeringDelta = STEERING_SPEED * deltaT;
    currentSteeringAngle = glm::clamp(
        currentSteeringAngle + glm::clamp(targetSteeringAngle - currentSteeringAngle, -steeringDelta, steeringDelta),
        -MAX_STEERING_ANGLE, MAX_STEERING_ANGLE);

    // Update car position and rotation using Ackermann steering
    if (std::abs(carSpeed) > 0.001f)
    {
        float turnRadius = WHEELBASE / std::tan(std::abs(currentSteeringAngle) + 1e-5);
        float angularVelocity = carSpeed / turnRadius;

        carRotation += angularVelocity * deltaT * glm::sign(currentSteeringAngle);
        carPosition += glm::vec3(
            std::cos(carRotation) * carSpeed * deltaT,
            0.0f,
            std::sin(carRotation) * carSpeed * deltaT);
    }

    // Tilt the car based on amount of acceleration
    float carTilt = glm::radians(glm::clamp(m.z * -5.0f, -5.0f, 5.0f));

    // Update car matrix
    CarPos = glm::translate(glm::mat4(1.0f), carPosition);
    CarPos = glm::rotate(CarPos, -carRotation, glm::vec3(0.0f, 1.0f, 0.0f));
    CarPos = glm::rotate(CarPos, -carTilt, glm::vec3(0.0f, 0.0f, 1.0f));
    // rotate model by 90 degrees clockwise and scale it 3x
    CarPos = glm::scale(CarPos, glm::vec3(3.0f, 3.0f, 3.0f));
    CarPos = glm::rotate(CarPos, glm::radians(90.0f), glm::vec3(0.0f, -1.0f, 0.0f));
}