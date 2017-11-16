//
// Created by Morten Nobel-Jørgensen on 11/6/17.
//

#include <iostream>
#include "BirdMovementComponent.hpp"
#include "GameObject.hpp"

BirdMovementComponent::BirdMovementComponent(GameObject *gameObject) : Component(gameObject) {}

void BirdMovementComponent::update(float deltaTime) {
    time += deltaTime;
    gameObject->setPosition(computePositionAtTime(time));
}

glm::vec2 BirdMovementComponent::computePositionAtTime(float time) {
    int segment = (int)fmod(time, getNumberOfSegments());
    float t = fmod(time,1.0f);

	// Calculate the middle points (actual trajectory points) by getting the middle of each line segment: 
	glm::vec2 p0 = positions[segment]; 
	glm::vec2 p1 = positions[segment+1]; 
	glm::vec2 p2 = positions[segment + 2]; 

	glm::vec2 start = glm::mix(p0, p1, 0.5f);
	glm::vec2 end = glm::mix(p1, p2, 0.5f); 

	glm::vec2 control = p1; 

	// Start is p0, end is p2, control is p1
	float posx = (1 - t) * (1 - t) * start.x + 2 * (1 - t) * t * control.x + t * t * end.x;
	float posy = (1 - t) * (1 - t) * start.y + 2 * (1 - t) * t * control.y + t * t * end.y;

	return glm::vec2(posx, posy); 

    // return glm::mix(positions[segment],positions[segment+1],t);

}

const std::vector<glm::vec2> &BirdMovementComponent::getPositions() {
    return positions;
}

void BirdMovementComponent::setPositions(std::vector<glm::vec2> positions) {
    this->positions = std::move(positions);
}

int BirdMovementComponent::getNumberOfSegments() {
    // todo return number of Quadratic Bézier spline segments instead
    return positions.size()-2;
}

