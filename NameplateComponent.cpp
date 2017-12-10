#include "NameplateComponent.h"
#include "GameObject.hpp"

NameplateComponent::NameplateComponent(GameObject *gameObject) : Component(gameObject)
{
}

/* 
* Used to initiate a Nameplate Component
* const char* nameplateString: The name that appears above the object.
* float wWidth: The width of the Games Window
* float wHeight: The height of the Games Window
* float offsetY: The Y-Offset of the Nameplate in Screenspace
* ImVec4 color: The color of the text
* SideScrollingCamera* camRef: The main camera used by the game. Necessary to render things in Screenspace.
*/
void NameplateComponent::init(const char* nameplateString, float wWidth, float wHeight, float offsetY, ImVec4 color, SideScrollingCamera* camRef)
{
	name = nameplateString;
	windowWidth = wWidth;
	windowHeight = wHeight;
	this->offsetY = offsetY;
	nameplateColor = color;
	nameplateSize = ImVec2(8, 8);
	cameraRef = &camRef->getCamera();
}

void NameplateComponent::update(float deltaTime)
{
}

void NameplateComponent::render()
{
	if (ShowPlate == true)
	{
		float x = gameObject->getPosition().x;
		float y = gameObject->getPosition().y;

		glm::vec4 ws_point = glm::vec4(x, y, 0.0f, 1.0f);
		glm::vec4 cspoint = cameraRef->getProjectionTransform(glm::uvec2(windowWidth, windowHeight)) * cameraRef->getViewTransform() * ws_point;
		glm::vec4 windowsCoord = (glm::vec4((cspoint.x * 0.5f) + 0.5f, (cspoint.y * 0.5f) + 0.5f, 0.0f, 1.0f));
		float nameplateX = windowsCoord.x * windowWidth - (nameplateSize.x * 4);
		float nameplateY = windowHeight - (windowsCoord.y * windowHeight) - nameplateSize.y - offsetY;
		ImGui::SetNextWindowPos(ImVec2(nameplateX, nameplateY));
		bool open = true;
		ImGui::Begin(name, &open, nameplateSize, 0.5f, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar);
		ImGui::TextColored(nameplateColor, name);
		ImGui::End();
	}	
}