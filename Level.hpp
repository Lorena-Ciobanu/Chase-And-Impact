#pragma once

#include <memory>
#include <vector>
#include <sre/SpriteAtlas.hpp>

class ChaseAndImpactGame;
class GameObject;
class PlatformComponent;

enum class TileCollider {
    Block,
    BlockHalf,
    Incline,
    Decline
};

class Level {
public:
    static std::shared_ptr<Level> createDefaultLevel(ChaseAndImpactGame* game, std::shared_ptr<sre::SpriteAtlas> spriteAtlas);

    void generateLevel();
    std::shared_ptr<PlatformComponent> addPlatform(int x, int y, int startSpriteId, int length, bool kinematic);
    std::shared_ptr<PlatformComponent> addWall(int x, int y, int startSpriteId, int height);

    static constexpr float tileSize = 21;
private:
    Level() = default;
    ChaseAndImpactGame* game;
    std::shared_ptr<sre::SpriteAtlas> spriteAtlas;
};

