//
// Created by Jaime Monedero March on 16/11/17.
//

#pragma once

#include "Component.hpp"
#include <vector>
#include "glm/glm.hpp"

class BoulderMovementComponent : public Component {
public:
    explicit BoulderMovementComponent(GameObject *gameObject);

    void update(float deltaTime) override;

	bool CanMove = true;
	void init(float physicsScale, float radius, glm::vec2 center, float density);

	std::shared_ptr<PhysicsComponent> boulderPhysics;

private:
    float time = 0;
	float speed = 5.0f; 
	float rotationSpeeed = 40.0f;
	float initialx = 0.0f;
	float initialy = 2.12f;

	/* Physics */
	float radius;
	float maximumVelocity;
	

	
	//void stopMovement();

};

