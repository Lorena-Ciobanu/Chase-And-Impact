//
// Created by Jaime Monedero March on 16/11/17.
//

#include <iostream>
#include "BoulderMovementComponent.hpp"
#include "GameObject.hpp"
#include "Level.hpp"
#include "PhysicsComponent.hpp"

BoulderMovementComponent::BoulderMovementComponent(GameObject *gameObject) : Component(gameObject) {
	boulderPhysics = gameObject->addComponent<PhysicsComponent>();
}

void BoulderMovementComponent::init(float physicsScale, float radius, glm::vec2 center, float density)
{
	radius /= physicsScale;
	boulderPhysics->initCircle(b2BodyType::b2_dynamicBody, radius, glm::vec2(initialx, initialy), 1.0f);

}


void BoulderMovementComponent::update(float deltaTime) {
	/* if (CanMove == true)
	{
		time += deltaTime;
		gameObject->setPosition(glm::vec2((speed*time + initialx)*Level::tileSize, initialy*Level::tileSize));
	}
	gameObject->setRotation(gameObject->getRotation() - (deltaTime* this->rotationSpeeed)); */
}

