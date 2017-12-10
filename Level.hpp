#pragma once

#include <memory>
#include <vector>
#include <sre/SpriteAtlas.hpp>
#include <queue>

class ChaseAndImpactGame;
class GameObject;
class PlatformComponent;
class LevelSection;

enum class TileCollider {
    Block,
    BlockHalf,
    Incline,
    Decline
};

class Level {
public:
	Level(ChaseAndImpactGame* game, std::shared_ptr<sre::SpriteAtlas> spriteAtlas);

    static constexpr float tileSize = 21;

	static constexpr float endLevelOffset = 150;		// how much the boulder has to roll after the section for it to be destoyed [so that it's not visible]

	void generateLevel();

	void updateLevel();

	float getCurrentUpdatePosition();

	~Level();

private:
	int const initialNrOfSections = 3;

	float currentSectionIndex = 0;

	void addSection();

	void removeSection();

    ChaseAndImpactGame* game;
    std::shared_ptr<sre::SpriteAtlas> spriteAtlas;

	std::queue<std::shared_ptr<LevelSection>> levelSections;
};

