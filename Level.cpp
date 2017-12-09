#include "Level.hpp"
#include "ChaseAndImpactGame.hpp"
#include "LevelSection.h"

Level::Level(ChaseAndImpactGame * game, std::shared_ptr<sre::SpriteAtlas> spriteAtlas)
{
	this->game = game;
	this->spriteAtlas = spriteAtlas;
}

void Level::generateLevel()
{
	for (int i = 0; i < initialNrOfSections; i++) {
		addSection();
	}
}

void Level::updateLevel()
{
	removeSection();
	addSection();
}

float Level::getCurrentUpdatePosition()
{
	return levelSections.front()->getEndBound();
}

void Level::addSection()
{
	levelSections.emplace(std::make_shared<LevelSection>(game, spriteAtlas, currentSectionIndex++));
}

void Level::removeSection()
{
	levelSections.pop();
}

Level::~Level()
{
	while (!levelSections.empty()) {
		levelSections.pop();
	}
}


/*std::shared_ptr<Level> Level::createDefaultLevel(ChaseAndImpactGame* game, std::shared_ptr<sre::SpriteAtlas> spriteAtlas) {
    std::shared_ptr<Level> res = std::shared_ptr<Level>(new Level());

    res->game = game;
    res->spriteAtlas = spriteAtlas;

    return res;
}

void Level::generateLevel() {
    // start wall
	addWall(boundryLeftX, 1, 2, 20);

	generateSection(boundryLeftX, boundryRightX);

    // end wall
    addWall(100, 1, 2, 20);
}*/

/*std::shared_ptr<PlatformComponent> Level::addPlatform(int x, int y, int startSpriteId, int length, bool kinematic) {
    auto gameObject = game->createGameObject();
    gameObject->name = "Platform";
    auto res = gameObject->addComponent<PlatformComponent>();
    res->init(spriteAtlas, x,y,startSpriteId, length, kinematic);
    return res;
}


std::shared_ptr<PlatformComponent> Level::addWall(int x, int y, int startSpriteId, int length) {
    auto gameObject = game->createGameObject();
    gameObject->name = "Platform";
    auto res = gameObject->addComponent<PlatformComponent>();
    res->initWall(spriteAtlas, x,y,startSpriteId, length);
    return res;
} */

/*void Level::generateSection(int startX, int endX)
{
	// floor
	addPlatform(startX, boundryBottomY, 2, sectionLength, false);

	// ceil
	addPlatform(startX, boundryTopY, 2, sectionLength, false);

	/*  auto movingPlatform = addPlatform(10, 3, 2, 5, true);
	auto movingPlatformComponent = movingPlatform->getGameObject()->addComponent<MovingPlatformComponent>();
	movingPlatformComponent->setMovementStart({10,3});
	movingPlatformComponent->setMovementEnd({10,5}); 

	// add some more platforms
	addPlatform(15, 7, 2, 5, true);
	addPlatform(20, 3, 2, 5, true);
	addPlatform(25, 7, 2, 5, true);
	addPlatform(30, 10, 2, 5, true);
	addPlatform(35, 7, 2, 5, true);
	addPlatform(40, 3, 2, 5, true);
}
*/


