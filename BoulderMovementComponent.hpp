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

	void BoulderMovementComponent::onCollisionStart(PhysicsComponent *comp);

    void update(float deltaTime) override;

	bool CanMove = true;
	void init(float physicsScale, float radius, glm::vec2 center, float density);

	std::shared_ptr<PhysicsComponent> boulderPhysics;

	void setGameInstance(ChaseAndImpactGame* instance);

	~BoulderMovementComponent();

private:
    float time = 0;
	float maxSpeed = 4.0f; 
	float maxRotationSpeeed = 40.0f;
	float initialx = 0.0f;
	float initialy = 2.12f;

	/* Physics */
	float radius;
	float maximumVelocity;
	

	ChaseAndImpactGame* game;
	
	//void stopMovement();

};

