#include "sre/SDLRenderer.hpp"
#include "sre/SpriteAtlas.hpp"
#include <vector>
#include "Box2D/Dynamics/b2World.h"
#include "GameObject.hpp"
#include "SideScrollingCamera.hpp"
#include "Box2DDebugDraw.hpp"
#include "Level.hpp"

class PhysicsComponent;

enum class GameState{
    Ready,
    Running,
    GameOver
};

class ChaseAndImpactGame : public b2ContactListener {
public:
    ChaseAndImpactGame();

    std::shared_ptr<GameObject> createGameObject();
    static const glm::vec2 windowSize;

    void BeginContact(b2Contact *contact) override;

    void EndContact(b2Contact *contact) override;

    static ChaseAndImpactGame* instance;

    static constexpr float32 timeStep = 1.0f / 60.0f;
	
private:
    sre::SDLRenderer r;

    void initLevel();
    void initPhysics();

	void initPlayerObject(std::string playerName, int spriteAtlasStartIndex, glm::vec2 startPosition,
						SDL_Keycode upKey, SDL_Keycode leftKey, SDL_Keycode rightKey);

    void update(float time);

    void render();

    void onKey(SDL_Event &event);

    void handleContact(b2Contact *contact, bool begin);

    std::shared_ptr<SideScrollingCamera> camera;
    std::shared_ptr<sre::SpriteAtlas> spriteAtlas;

    std::vector<std::shared_ptr<GameObject>> sceneObjects;

    void updatePhysics();

    std::shared_ptr<Level> level;
    glm::vec4 backgroundColor;
    b2World * world = nullptr;
	const float physicsScale = 100;
    void registerPhysicsComponent(PhysicsComponent *r);
    void deregisterPhysicsComponent(PhysicsComponent *r);
    std::map<b2Fixture*,PhysicsComponent *> physicsComponentLookup;
    Box2DDebugDraw debugDraw;
    bool doDebugDraw = false;
    friend class PhysicsComponent;
    friend class Level;
    friend class CharacterControllerComponent;
    friend class PlatformComponent;
};

