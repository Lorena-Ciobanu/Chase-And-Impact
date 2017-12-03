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
private:
    float time = 0;
	float speed = 5.0f; 
	float rotationSpeeed = 50.0f;
	float initialx = 0.0f;
	float initialy = 7.1f;
};

