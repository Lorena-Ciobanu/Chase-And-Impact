//
// Created by Jaime Monedero March on 16/11/17.
//

#include <iostream>
#include "ChaseAndImpactGame.hpp"
#include "BoulderMovementComponent.hpp"
#include "GameObject.hpp"
#include "Level.hpp"
#include "PhysicsComponent.hpp"
#include "SpriteComponent.hpp"

BoulderMovementComponent::BoulderMovementComponent(GameObject *gameObject) : Component(gameObject) {
	boulderPhysics = gameObject->addComponent<PhysicsComponent>();
}

void BoulderMovementComponent::init(float physicsScale, float radius, glm::vec2 center, float density)
{
	this->radius = radius / physicsScale;
	boulderPhysics->initCircle(b2BodyType::b2_kinematicBody, this->radius, glm::vec2(initialx, initialy), 10.0f);
	boulderPhysics->setSensor(true); 
	
}


void BoulderMovementComponent::update(float deltaTime) {

	if(CanMove){

		/* Angular Velocity */
		float angularVel = boulderPhysics->getAngularVelocity();
		angularVel = b2Max(angularVel - 0.5f, -maxRotationSpeeed);
		boulderPhysics->setAngularVelocity(angularVel);

		/* Linear Velocity */
		auto vel = boulderPhysics->getLinearVelocity();
		vel.x = b2Min(vel.x + 0.008f, maxSpeed);
		boulderPhysics->setLinearVelocity(vel);
	}
	else {
		boulderPhysics->setAngularVelocity(0);
		boulderPhysics->setLinearVelocity(glm::vec2(0,0));
	}
}

/*
void BoulderMovementComponent::update(float deltaTime) {
	 if (CanMove == true)
	{
		time += deltaTime;
		gameObject->setPosition(glm::vec2((speed*time + initialx)*Level::tileSize, initialy*Level::tileSize));
	}
	gameObject->setRotation(gameObject->getRotation() - (deltaTime* this->rotationSpeeed)); 
} 
*/

void BoulderMovementComponent::onCollisionStart(PhysicsComponent *comp) {

	auto compName = comp->getGameObject()->name;

	if (compName == "Player 1" || compName == "Player 2")
	{
		auto playerSpriteRenderer = comp->getGameObject()->getComponent<SpriteComponent>();
		comp->getGameObject()->removeComponent(playerSpriteRenderer);
		// TODO Maybe add a particle effect

		ChaseAndImpactGame::endGame(compName); 
	}

}
