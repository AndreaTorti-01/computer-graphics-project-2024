#include "Car.hpp"
#include <gtc/matrix_transform.hpp>

Car::Car()
{
	maxSpeed = 8.0f;
	acceleration = 5.0f;
	deceleration = 20.0f;
	maxSteeringAngle = 0.7f;
	steeringSpeed = 3.2f;
	wheelbase = 2.0f;
	shootCooldown = 0.4f;
	timeSinceLastShot = 0.0f;
    carSpeed = 0.0f;
    currentSteeringAngle = 0.0f;
}

void Car::update(float deltaTime, const glm::vec3& controls)
{
    // Update car speed
    if (controls.z > 0.1f)
    {
        if (carSpeed < 0)
        {
            // If moving backward and forward button is pressed, decelerate using deceleration
            carSpeed = std::min(carSpeed + deceleration * deltaTime, 0.0f);
        }
        else
        {
            carSpeed = std::min(carSpeed + acceleration * deltaTime, maxSpeed);
        }
    }
    else if (controls.z < -0.1f)
    {
        if (carSpeed > 0)
        {
            // If moving forward and backward button is pressed, decelerate using deceleration
            carSpeed = std::max(carSpeed - deceleration * deltaTime, 0.0f);
        }
        else
        {
            carSpeed = std::max(carSpeed - acceleration * deltaTime, -maxSpeed);
        }
    }
    else
    {
        // Apply natural deceleration when no input
        carSpeed = carSpeed > 0 ? std::max(carSpeed - acceleration * deltaTime, 0.0f) : carSpeed < 0 ? std::min(carSpeed + acceleration * deltaTime, 0.0f)
            : 0.0f;
    }

    // Update steering angle incrementally
    float targetSteeringAngle = -maxSteeringAngle * controls.x;
    float steeringDelta = steeringSpeed * deltaTime;
    currentSteeringAngle = glm::clamp(
        currentSteeringAngle + glm::clamp(targetSteeringAngle - currentSteeringAngle, -steeringDelta, steeringDelta),
        -maxSteeringAngle, maxSteeringAngle);

    // Update car position and rotation using Ackermann steering
    if (std::abs(carSpeed) > 0.001f)
    {
        float turnRadius = wheelbase / std::tan(std::abs(currentSteeringAngle) + 1e-5);
        float angularVelocity = carSpeed / turnRadius;

        rotation += angularVelocity * deltaTime * glm::sign(currentSteeringAngle);
        position += glm::vec3(
            std::cos(rotation) * carSpeed * deltaTime,
            0.0f,
            std::sin(rotation) * carSpeed * deltaTime);
    }

    // Constrain the car position to the floor size
    const float floorDiamAdj = (FLOOR_DIAM - 1.0f) / 2.0f;
    position.x = glm::clamp(position.x, -floorDiamAdj, floorDiamAdj);
    position.z = glm::clamp(position.z, -floorDiamAdj, floorDiamAdj);

    // Tilt the car based on amount of acceleration
    float carTilt = glm::radians(glm::clamp(controls.z * -5.0f, -5.0f, 5.0f));

    /*
    // Update car matrix
    CarPos = glm::translate(glm::mat4(1.0f), position);
    CarPos = glm::rotate(CarPos, -rotation, glm::vec3(0.0f, 1.0f, 0.0f));
    CarPos = glm::rotate(CarPos, -carTilt, glm::vec3(0.0f, 0.0f, 1.0f));
    // rotate model by 90 degrees clockwise and scale it 3x
    CarPos = glm::scale(CarPos, glm::vec3(3.0f, 3.0f, 3.0f));
    CarPos = glm::rotate(CarPos, glm::radians(90.0f), glm::vec3(0.0f, -1.0f, 0.0f));
    */

    // Check if it's time to shoot
    timeSinceLastShot += deltaTime;
    if (timeSinceLastShot >= shootCooldown)
	{
		timeSinceLastShot = 0.0f;
        // Shoot a bullet
		for (auto &bullet : bullets)
		{
			if (!bullet.getIsAboveFloor())
			{
				bullet.shoot(position, rotation);
				break;
			}
		}
	}
}
