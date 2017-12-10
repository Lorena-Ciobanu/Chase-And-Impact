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

void Level::resetLevel()
{
	while (!levelSections.empty()) {
		levelSections.pop();
	}
	currentSectionIndex = 0;
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


