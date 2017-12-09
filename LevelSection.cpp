#include "LevelSection.h"
#include "ChaseAndImpactGame.hpp"
#include "SpriteComponent.hpp"
#include "PhysicsComponent.hpp"
#include "PlatformComponent.hpp"


LevelSection::LevelSection(ChaseAndImpactGame * game, std::shared_ptr<sre::SpriteAtlas> spriteAtlas, float sectionIndex)
{
	this->game = game;
	this->spriteAtlas = spriteAtlas;

	this->boundryLeft = sectionIndex * sectionLength + initialOffset;
	this->boundryRight = boundryLeft + sectionLength + initialOffset;

	generateLevelSection();
}

float LevelSection::getEndBound()
{
	float tileSize = Level::tileSize;
	float offset = tileSize / 2;
	return offset + boundryRight*tileSize;
}



void LevelSection::generateLevelSection()
{
	generateCeiling();
	generateFloor();

	// for (some random number){
	// generatePlatforms()
}


void LevelSection::generateFloor()
{
	platforms.push_back(createPlatform("Floor", boundryLeft, boundryBottom, sectionLength));
}


void LevelSection::generateCeiling()
{
	platforms.push_back(createPlatform("Ceiling", boundryLeft, boundryTop, sectionLength));
}

void LevelSection::generateObstacles()
{
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

LevelSection::~LevelSection()
{
	if (!platforms.empty()) {
		platforms.clear();
	}
}
