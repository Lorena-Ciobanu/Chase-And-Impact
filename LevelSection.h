#pragma once

#include <sre/SpriteAtlas.hpp>
#include <memory>
#include<vector>

class ChaseAndImpactGame;
class GameObject;
class PlatformComponent;

class LevelSection {
public:
	LevelSection(ChaseAndImpactGame* game, std::shared_ptr<sre::SpriteAtlas> spriteAtlas, float sectionIndex);

	float getEndBound();

	~LevelSection();

private:
	ChaseAndImpactGame* game;
	std::shared_ptr<sre::SpriteAtlas> spriteAtlas;

	float const sectionLength = 25.0f;
	float const initialOffset = -3;		//to account for the size of the boulder on the first few frames
	int const spriteId = 2;

	float boundryLeft;
	float boundryRight;

	int boundryTop = 20;
	int boundryBottom = 0;


	std::vector<std::shared_ptr<GameObject>> platforms;

	void generateLevelSection();

	void generateFloor();

	void generateCeiling();

	void generateObstacles();


	std::shared_ptr<GameObject> createObject(std::string name);

	std::shared_ptr<GameObject> createPlatform(std::string name, int x, int y, int length);

	std::shared_ptr<GameObject> createWall(std::string name, int x, int y, int length);

};