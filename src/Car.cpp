#include "Car.hpp"

Car::Car()
{
    maxSpeed = 8.0f;
    acceleration = 5.0f;
    deceleration = 20.0f;
    maxSteeringAngle = 0.7f;
    steeringSpeed = 3.2f;
    wheelbase = 2.0f;
    shootCooldown = 2.0f;
    timeSinceLastShot = 0.0f;
    carSpeed = 0.0f;
    currentSteeringAngle = 0.0f;
    health = 5;
    score = 0;
    position = glm::vec3(0.0f, 0.0f, 0.0f);
    position4 = glm::mat4(1.0f);
    rotation = 0.0f;
}

float Car::getSpeed() { return carSpeed; }

int Car::getHealth() { return health; }

int Car::getScore() { return score; }

glm::mat4 Car::getPosition4() { return position4; }

void Car::setSinceLastShot(float time) { timeSinceLastShot = time; }

float Car::getSinceLastShot() { return timeSinceLastShot; }

std::array<Bullet, MAX_BULLET_INSTANCES> Car::getBullets() { return bullets; }

void Car::damage()
{
	health--;
}

void Car::update(float deltaT, glm::vec3& controls)
{
	// Car properties

	const float MAX_SPEED = 8.0f;
	const float ACCELERATION = 5.0f;
	const float DECELERATION = 20.0f;
	const float MAX_STEERING_ANGLE = glm::radians(35.0f);
	const float STEERING_SPEED = glm::radians(180.0f);
	const float WHEELBASE = 2.0f;

	// Update car speed
	if (controls.z > 0.1f)
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
	else if (controls.z < -0.1f)
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
	float targetSteeringAngle = -MAX_STEERING_ANGLE * controls.x;
	float steeringDelta = STEERING_SPEED * deltaT;
	currentSteeringAngle = glm::clamp(
		currentSteeringAngle + glm::clamp(targetSteeringAngle - currentSteeringAngle, -steeringDelta, steeringDelta),
		-MAX_STEERING_ANGLE, MAX_STEERING_ANGLE);

	// Update car position and rotation using Ackermann steering
	if (std::abs(carSpeed) > 0.001f)
	{
		float turnRadius = WHEELBASE / std::tan(std::abs(currentSteeringAngle) + 1e-5);
		float angularVelocity = carSpeed / turnRadius;

		rotation += angularVelocity * deltaT * glm::sign(currentSteeringAngle);
		position += glm::vec3(
			std::cos(rotation) * carSpeed * deltaT,
			0.0f,
			std::sin(rotation) * carSpeed * deltaT);
	}

	// Constrain the car position to the floor size
	const float floorDiamAdj = (FLOOR_DIAM - 1.0f) / 2.0f;
	position.x = glm::clamp(position.x, -floorDiamAdj, floorDiamAdj);
	position.z = glm::clamp(position.z, -floorDiamAdj, floorDiamAdj);
	position.y = 0.0f;

	// Tilt the car based on amount of acceleration
	float carTilt = glm::radians(glm::clamp(controls.z * -2.0f, -2.0f, 2.0f));

    // Update car matrix
    position4 = glm::translate(glm::mat4(1.0f), position);
    position4 = glm::rotate(position4, -rotation, glm::vec3(0.0f, 1.0f, 0.0f));
    position4 = glm::rotate(position4, -carTilt, glm::vec3(0.0f, 0.0f, 1.0f));
    // rotate model by 90 degrees clockwise and scale it down 3x
    position4 = glm::scale(position4, glm::vec3(0.3));
    position4 = glm::rotate(position4, glm::radians(90.0f), glm::vec3(0.0f, -1.0f, 0.0f));

	// Update bullets
	timeSinceLastShot += deltaT;

    for (auto &bullet : bullets)
    {
        if (bullet.getIsAboveFloor())
        {
            bullet.update(deltaT);
        }
        else
        {
            if (timeSinceLastShot >= shootCooldown)
            {
                bullet.shoot(position, rotation, glm::vec3(position4[2]));
                timeSinceLastShot = 0.0f;
            }
        }
    }
}

void Car::check_collisions(std::array<Mike, MAX_MIKE_INSTANCES> &mikes, std::array<Upgrade, MAX_UPGRADE_INSTANCES> &upgrades)
{
    for (auto &up : upgrades)
    {
        if(glm::distance(up.getPosition(), position) <= 0.8)
        {
            switch(up.getUpgrade()){
                case 0:
                    for(auto &bullet : bullets){
                        shootCooldown = glm::clamp(shootCooldown - 0.1, 0.2, 2.0);
                    }
                    break;
                case 1:
                    for(auto &bullet : bullets){
                        bullet.increaseSpeed();
                    }
                    break;
            }
        }
    }
    for (auto &mike : mikes)
    {
        if (glm::distance(mike.getPosition(), position) <= 0.5)
        {
            mike.reset();
            damage();
        }

        for (auto &bullet : bullets)
        {
            if (mike.getIsAboveFloor() && !mike.getDamaged() && bullet.getIsAboveFloor())
            {
                if (glm::distance(mike.getPosition(), bullet.getPosition()) <= 1.0)
                {
                    mike.setDamaged(true);
                    mike.setDamageTimer(0.0f);
                    bullet.explode();
                    score++;
                }
            }
        }
    }
}
