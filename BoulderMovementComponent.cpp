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

void BoulderMovementComponent::setGameInstance(ChaseAndImpactGame * instance)
{
	game = instance;
}


void BoulderMovementComponent::update(float deltaTime) {

	if(CanMove){

		/* Angular Velocity */
		float angularVel = boulderPhysics->getAngularVelocity();
		angularVel = b2Max(angularVel - 0.5f, -maxRotationSpeeed);
		boulderPhysics->setAngularVelocity(angularVel);

		/* Linear Velocity */
		auto vel = boulderPhysics->getLinearVelocity();
		vel.x = b2Min(vel.x + 0.009f, maxSpeed);
		boulderPhysics->setLinearVelocity(vel);


		/* Check whether or not to destroy previous level sections*/
		auto currentBoulderPos = gameObject->getPosition();
		auto currentUpdatePos = game->getCurrentUpdatePosition();

		if (currentBoulderPos.x >= currentUpdatePos + Level::endLevelOffset) {
			game->updateLevel();
		}
	}
	else {
		boulderPhysics->setAngularVelocity(0);
		boulderPhysics->setLinearVelocity(glm::vec2(0,0));
	}
}

void BoulderMovementComponent::onCollisionStart(PhysicsComponent *comp) {

	auto compName = comp->getGameObject()->name;

	if (compName == "Player 1" || compName == "Player 2")
	{
		auto playerSpriteRenderer = comp->getGameObject()->getComponent<SpriteComponent>();
		comp->getGameObject()->removeComponent(playerSpriteRenderer);

		// TODO Maybe add a particle effect
		CanMove = false;

		game->endGame(compName); 
	}

}
