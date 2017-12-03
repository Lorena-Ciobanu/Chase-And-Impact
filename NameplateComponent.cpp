#include "NameplateComponent.h"
#include "GameObject.hpp"

NameplateComponent::NameplateComponent(GameObject *gameObject) : Component(gameObject)
{
}

void NameplateComponent::init(const char* nameplateString, float wWidth, float wHeight, SideScrollingCamera* camRef)
{
	name = nameplateString;
	windowWidth = wWidth;
	windowHeight = wHeight;
	nameplateSize = ImVec2(16, 16);
	cameraRef = &camRef->getCamera();
}

void NameplateComponent::update(float deltaTime)
{
}

void NameplateComponent::render()
{
	float x = gameObject->getPosition().x;
	float y = gameObject->getPosition().y;

	glm::vec4 ws_point = glm::vec4(x, y, 0.0f, 1.0f);
	glm::vec4 cspoint = cameraRef->getProjectionTransform(glm::uvec2(windowWidth, windowHeight)) * cameraRef->getViewTransform() * ws_point;
	glm::vec4 windowsCoord = (glm::vec4((cspoint.x * 0.5f) + 0.5f, (cspoint.y * 0.5f) + 0.5f, 0.0f, 1.0f));
	float xx = windowsCoord.x * windowWidth;
	float yy = windowsCoord.y * windowHeight;
	ImGui::SetNextWindowPos(ImVec2(xx,windowHeight), ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoInputs);
	bool open = true;
	ImGui::Begin("#TestLabel", &open, nameplateSize, 0);
	ImGui::TextColored(ImVec4(255, 0, 255, 1), name);
	ImGui::End();
}