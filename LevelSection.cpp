#include "LevelSection.h"
#include "ChaseAndImpactGame.hpp"
#include "SpriteComponent.hpp"
#include "PhysicsComponent.hpp"
#include "PlatformComponent.hpp"

#include <algorithm>


LevelSection::LevelSection(ChaseAndImpactGame * game, std::shared_ptr<sre::SpriteAtlas> spriteAtlas, float sectionIndex)
{
	this->game = game;
	this->spriteAtlas = spriteAtlas;

	this->boundryLeft = sectionIndex * sectionLength + initialOffset;
	this->boundryRight = boundryLeft + sectionLength + initialOffset;

	float tileSize = Level::tileSize;
	float offset = Level::tileSize / 2;
	worldSpaceBoundryLeft = offset + boundryLeft*tileSize;
	worldSpaceBoundyRight= offset + boundryRight*tileSize;

	generateLevelSection();
}

float LevelSection::getEndBound()
{
	return worldSpaceBoundyRight;
}


void LevelSection::generateLevelSection()
{
	generateCeiling();
	generateFloor();

	if (generatePlaftforms) {
		currentPlatformEndX = boundryLeft;
		currentPlatformEndY = getRandomNumberInRange(minJumpDistanceY, maxWallLength);
		generateWall(currentPlatformEndX, boundryBottom + 1, currentPlatformEndY);
	
		generateObstacles();
	}
}


void LevelSection::generateFloor()
{
	platforms.push_back(createPlatform("Floor", boundryLeft, boundryBottom, sectionLength));
}


void LevelSection::generateCeiling()
{
	platforms.push_back(createPlatform("Ceiling", boundryLeft, boundryTop, sectionLength));
}

void LevelSection::generatePlatform(int x, int y, int length)
{
	platforms.push_back(createPlatform("Platform", x, y, length));
}

void LevelSection::generateWall(int x, int y, int length) {
	platforms.push_back(createWall("Wall", x, y, length));
}

void LevelSection::generateObstacles()
{
	
	while (currentPlatformEndX < boundryRight) {
		float length = getRandomNumberInRange(minPlatformLength, maxPlatformLenght);

		currentPlatformEndX = currentPlatformEndX + getRandomNumberInRange(minJumpDistanceX, maxJumpDistanceX);
		currentPlatformEndY = currentPlatformEndY + getRandomNumberInRange(minJumpDistanceY, maxJumpDistanceY);
		
		float isPlatfomAbove = getRandomNumberInRange(0, 1);
		if (!isPlatfomAbove) {
			currentPlatformEndY *= -1;
		}


		if (currentPlatformEndY > boundryTop) {
			currentPlatformEndY = boundryTop - getRandomNumberInRange(minJumpDistanceX, maxJumpDistanceX);
		}

		if (currentPlatformEndY < boundryBottom) {
			currentPlatformEndY = boundryBottom + getRandomNumberInRange(minJumpDistanceY, maxJumpDistanceY);
		}

		if (currentPlatformEndX + length >= boundryRight) {
			break;
		}
		generatePlatform(currentPlatformEndX, currentPlatformEndY, length);

		currentPlatformEndX += length;
	}

}


std::shared_ptr<GameObject> LevelSection::createObject(std::string name)
{
	auto gameObject = game->createGameObject();
	gameObject->name = name;
	return gameObject;
}


std::shared_ptr<GameObject> LevelSection::createPlatform(std::string name, int x, int y, int length)
{
	auto gameObject = createObject(name);
	auto platform = gameObject->addComponent<PlatformComponent>();
	platform->init(spriteAtlas, x, y, spriteId, length, false);
	return gameObject;
}


std::shared_ptr<GameObject> LevelSection::createWall(std::string name, int x, int y, int length)
{
	auto gameObject = createObject(name);
	auto platform = gameObject->addComponent<PlatformComponent>();
	platform->initWall(spriteAtlas, x, y, spriteId, length);
	return gameObject;
}

int LevelSection::getRandomNumberInRange(int low, int high)
{
	return low + (std::rand() % (high - low + 1));
}

LevelSection::~LevelSection()
{
	for (int i = 0; i < platforms.size(); i++) {
		/*	
		Since out platforms are created out of a bigger number of tiles,
		each with their own gameObjectwe need to remove those as well
		*/

		auto platformComponent = platforms[i].get()->getComponent<PlatformComponent>();
		if (platformComponent) {
			auto tiles = platformComponent->getTiles();
			for (auto & tilesIt = tiles.begin(); tilesIt != tiles.end(); tilesIt++) {
				auto ptr = tilesIt->get();
				game->destroyGameObject(ptr);
			}

		}

		game->destroyGameObject(platforms[i].get());
	} 
}
