//
// Created by Morten Nobel-Jørgensen on 11/6/17.
//

#include "MovingPlatformComponent.hpp"
#include "PlatformComponent.hpp"
#include "GameObject.hpp"

MovingPlatformComponent::MovingPlatformComponent(GameObject *gameObject) : Component(gameObject)
{
	platformComponent = gameObject->getComponent<PlatformComponent>();
}

void MovingPlatformComponent::update(float deltaTime) {
	totalTime += deltaTime;
	float t = 0.0f;
	glm::vec2 pos;
	if (fmod(totalTime, fullCycleTime) < (fullCycleTime / 2)) {
		t = glm::smoothstep(0.0f, (fullCycleTime / 2), (float)fmod(totalTime, fullCycleTime));
		pos = glm::mix(movementStart, movementEnd, t);
	}
	else {
		t = glm::smoothstep(0.0f, (fullCycleTime / 2), (float)fmod(totalTime, fullCycleTime) - (fullCycleTime / 2));
		pos = glm::mix(movementEnd, movementStart, t);
	}
	platformComponent->moveTo(pos);
}

void MovingPlatformComponent::setMovementStart(glm::vec2 pos) {
	movementStart = pos;
}

void MovingPlatformComponent::setMovementEnd(glm::vec2 pos) {
	movementEnd = pos;
}
