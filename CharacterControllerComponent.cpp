//
// Created by Morten Nobel-Jørgensen on 19/10/2017.
//

#include <SDL_events.h>
#include <iostream>
#include "CharacterControllerComponent.hpp"
#include "GameObject.hpp"
#include "SpriteComponent.hpp"
#include "PhysicsComponent.hpp"
#include "ChaseAndImpactGame.hpp"
#include "SpriteComponent.hpp"

CharacterControllerComponent::CharacterControllerComponent(GameObject *gameObject, SDL_Keycode up, SDL_Keycode left, SDL_Keycode right) : Component(gameObject) {
	characterPhysics = gameObject->addComponent<PhysicsComponent>();
	UPKEY = up;
	LEFTKEY = left;
	RIGHTKEY = right;
	auto physicsScale = ChaseAndImpactGame::instance->physicsScale;
	radius = 10 / physicsScale;
	characterPhysics->initCircle(b2_dynamicBody, radius, glm::vec2{ 1.5,1.5 }*Level::tileSize / physicsScale, 1);
	characterPhysics->getFixture()->SetRestitution(0);
	characterPhysics->fixRotation();
	spriteComponent = gameObject->getComponent<SpriteComponent>();
}

bool CharacterControllerComponent::onKey(SDL_Event &event) {
	switch (event.key.keysym.sym) {
	case SDLK_SPACE:
	{
		if (isGrounded && event.type == SDL_KEYDOWN) { // prevents double jump
			jump();
		}
	}
	break;
	case SDLK_LEFT:
	{
		left = event.type == SDL_KEYDOWN;
	}
	break;
	case SDLK_RIGHT:
	{
		right = event.type == SDL_KEYDOWN;
	}
	break;
	}

	return false;
}

void CharacterControllerComponent::update(float deltaTime) {
	// raycast ignores any shape in the starting point
	auto from = characterPhysics->getBody()->GetWorldCenter();
	b2Vec2 to{ from.x,from.y - radius*1.3f };
	isGrounded = false;
	ChaseAndImpactGame::instance->world->RayCast(this, from, to);

	characterPhysics->fixRotation();
	glm::vec2 movement{ 0,0 };
	if (left) {
		movement.x--;
	}
	if (right) {
		movement.x++;
	}
	float accelerationSpeed = 0.010f;
	characterPhysics->addImpulse(movement*accelerationSpeed);
	auto linearVelocity = characterPhysics->getLinearVelocity();
	float currentVelocity = linearVelocity.x;
	if (abs(currentVelocity) > maximumVelocity) {
		linearVelocity.x = glm::sign(linearVelocity.x)*maximumVelocity;
		characterPhysics->setLinearVelocity(linearVelocity);
	}
	updateSprite(deltaTime);
}

void CharacterControllerComponent::jump() {
	characterPhysics->addImpulse({ 0,jumpForce });
}

void CharacterControllerComponent::onCollisionStart(PhysicsComponent *comp) {

}

void CharacterControllerComponent::onCollisionEnd(PhysicsComponent *comp) {

}

float32 CharacterControllerComponent::ReportFixture(b2Fixture *fixture, const b2Vec2 &point, const b2Vec2 &normal, float32 fraction) {
	isGrounded = true;
	return 0; // terminate raycast
}

void CharacterControllerComponent::setSprites(sre::Sprite standing, sre::Sprite walk1, sre::Sprite walk2, sre::Sprite flyUp,
	sre::Sprite fly, sre::Sprite flyDown) {
	this->standing = standing;
	this->walk1 = walk1;
	this->walk2 = walk2;
	this->flyUp = flyUp;
	this->fly = fly;
	this->flyDown = flyDown;
}

void CharacterControllerComponent::updateSprite(float deltaTime) {
	if (left)
	{
		direction = false;
	}
	if (right)
	{
		direction = true;
	}
	auto velocity = characterPhysics->getLinearVelocity();
	animationSpeed = 0.8 - ((glm::abs(velocity.x) / 2) * 0.8) + 0.2;
	if (isGrounded == true)
	{
		if (velocity == glm::vec2(0, 0))
		{
			spriteComponent->setSprite(standing);
		}
		else {
			animationDeltaTime += deltaTime;
			if (left == true)
			{
				if (animationDeltaTime >= animationSpeed)
				{
					animationDeltaTime = 0;
				}
				if (animationDeltaTime >= (animationSpeed / 2))
				{
					walk2.setFlip(glm::bvec2(true, false));
					spriteComponent->setSprite(walk2);
				}
				else {
					walk1.setFlip(glm::bvec2(true, false));
					spriteComponent->setSprite(walk1);
				}
			}
			else if (right == true) {
				if (animationDeltaTime >= animationSpeed)
				{
					animationDeltaTime = 0;
				}
				if (animationDeltaTime >= (animationSpeed / 2))
				{
					walk2.setFlip(glm::bvec2(false, false));
					spriteComponent->setSprite(walk2);
				}
				else {
					walk1.setFlip(glm::bvec2(false, false));
					spriteComponent->setSprite(walk1);
				}
			}
		}
	}
	else {
		if (velocity.y > 0.02f)
		{
			if (direction)
			{
				flyUp.setFlip(glm::bvec2(false, false));
			}
			else {
				flyUp.setFlip(glm::bvec2(true, false));
			}
			spriteComponent->setSprite(flyUp);
		}
		else if (velocity.y < -0.02f) {
			if (direction)
			{
				flyDown.setFlip(glm::bvec2(false, false));
			}
			else {
				flyDown.setFlip(glm::bvec2(true, false));
			}
			spriteComponent->setSprite(flyDown);
		}
		else {
			if (direction)
			{
				fly.setFlip(glm::bvec2(false, false));
			}
			else {
				fly.setFlip(glm::bvec2(true, false));
			}
			spriteComponent->setSprite(fly);
		}
	}
}