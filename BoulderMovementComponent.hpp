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

private:
    float time = 0;
	float speed = 2.0f; 
	float rotationSpeeed = 50.0f;
	float initialx = 0.0f;
	float initialy = 0.5f;

	/* Physics */
	float radius = 130.0f;
	float maximumVelocity;
	std::shared_ptr<PhysicsComponent> boulderPhysics;

	
	//void stopMovement();

};

