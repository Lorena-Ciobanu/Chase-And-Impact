//
// Created by Jaime Monedero March on 16/11/17.
//

#include <iostream>
#include "BoulderMovementComponent.hpp"
#include "GameObject.hpp"
#include "Level.hpp"

BoulderMovementComponent::BoulderMovementComponent(GameObject *gameObject) : Component(gameObject) {}

void BoulderMovementComponent::update(float deltaTime) {
	if (CanMove == true)
	{
		time += deltaTime;
		gameObject->setPosition(glm::vec2((speed*time + initialx)*Level::tileSize, initialy*Level::tileSize));
	}
}
