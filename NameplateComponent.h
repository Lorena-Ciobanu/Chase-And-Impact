#pragma once
#include "sre/SDLRenderer.hpp"
#include "SideScrollingCamera.hpp"
#include "sre/Camera.hpp"
#include "Component.hpp"

class NameplateComponent : public Component {
public:
	NameplateComponent(GameObject *gameObject);
	void init(const char* nameplateString, float windowWidth, float windowHeight, float offsetY, ImVec4 color, SideScrollingCamera* camRef);
	void update(float deltaTime);
	void render();
private:
	const char* name;
	float windowWidth;
	float windowHeight;
	float offsetY;
	ImVec2 nameplateSize;
	ImVec4 nameplateColor;
	sre::Camera* cameraRef;
};