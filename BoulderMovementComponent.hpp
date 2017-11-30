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
	float initialx = 1.5f;
	float initialy = 3.5f;
};

