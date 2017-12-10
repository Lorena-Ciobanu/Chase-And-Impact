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


	/* Section Generation */

	float const sectionLength = 30.0f;
	float const initialOffset = -3;		//to account for the size of the boulder on the first few frames
	int const spriteId = 2;

	float boundryLeft;
	float boundryRight;

	int boundryTop = 20;
	int boundryBottom = 0;

	float worldSpaceBoundryLeft;
	float worldSpaceBoundyRight;

	bool generatePlaftforms = true;


	/* Platforms Generation */

	std::vector<std::shared_ptr<GameObject>> platforms;
	int const minJumpDistanceX = 3;						/* Spawn platforms within jumping distance */
	int const maxJumpDistanceX = 5;
	
	int const minJumpDistanceY = 3;	
	int const maxJumpDistanceY = 6;

	int const maxWallLength = 4;

	float currentPlatformEndX;			// where within the section we are [should not be higher than the worldSpaceBoundyRight]
	float currentPlatformEndY;

	float minPlatformLength = 2;
	float maxPlatformLenght = 5;		// maximum platform length
	


	/* Functions */

	void generateLevelSection();

	void generateFloor();

	void generateCeiling();

	void generatePlatform(int x, int y, int length);

	void generateObstacles();


	std::shared_ptr<GameObject> createObject(std::string name);

	std::shared_ptr<GameObject> createPlatform(std::string name, int x, int y, int length);

	std::shared_ptr<GameObject> createWall(int x, int y, int length);


	/* Helper function to get a random number in range */
	int getRandomNumberInRange(int low, int high);
};