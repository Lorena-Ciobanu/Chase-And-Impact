#include <sre/Profiler.hpp>
#include "ChaseAndImpactGame.hpp"
#include "GameObject.hpp"
#include "sre/RenderPass.hpp"
#include "sre/Texture.hpp"
#include "SpriteComponent.hpp"
#include "Box2D/Dynamics/Contacts/b2Contact.h"
#include "PhysicsComponent.hpp"
#include "CharacterControllerComponent.hpp"

using namespace std;
using namespace sre;

const glm::vec2 ChaseAndImpactGame::windowSize(800,600);

ChaseAndImpactGame* ChaseAndImpactGame::instance = nullptr;

ChaseAndImpactGame::ChaseAndImpactGame()
        :debugDraw(physicsScale)
{
    instance = this;
    r.setWindowSize(windowSize);
    bool useVsync = true;
    r.init(SDL_INIT_EVERYTHING, SDL_WINDOW_OPENGL , useVsync);

    backgroundColor = glm::vec4(0.6,0.6,1,1);

    spriteAtlas = SpriteAtlas::create("platformer-art-deluxe.json","platformer-art-deluxe.png");

    level = Level::createDefaultLevel(this, spriteAtlas);

    initLevel();

    // setup callback functions
    r.keyEvent = [&](SDL_Event& e){
        onKey(e);
    };
    r.frameUpdate = [&](float deltaTime){
        update(deltaTime);
    };
    r.frameRender = [&](){
        render();
    };
    // start game loop
    r.startEventLoop();
}

void ChaseAndImpactGame::initLevel() {
    initPhysics();

    auto player = createGameObject();
    player->name = "Player";
    auto playerSprite = player->addComponent<SpriteComponent>();
    auto playerSpriteObj = spriteAtlas->get("19.png");
    playerSpriteObj.setPosition(glm::vec2{1.5,2.5}*Level::tileSize);
    playerSprite->setSprite(playerSpriteObj);
	SDL_Keycode keyupevent = SDL_Keycode(SDLK_UP);
	SDL_Keycode keyleftevent = SDL_Keycode(SDLK_LEFT);
	SDL_Keycode keyrightevent = SDL_Keycode(SDLK_RIGHT);
	auto characterController = player->addComponent<CharacterControllerComponent>();

	characterController->setKeyCodes(keyupevent, keyleftevent, keyrightevent);

    characterController->setSprites(
            spriteAtlas->get("19.png"),
            spriteAtlas->get("20.png"),
            spriteAtlas->get("21.png"),
            spriteAtlas->get("26.png"),
            spriteAtlas->get("27.png"),
            spriteAtlas->get("28.png")
    );

    auto camObj = createGameObject();
    camObj->name = "Camera";
    camera = camObj->addComponent<SideScrollingCamera>();
    camObj->setPosition(windowSize*0.5f);
    camera->setFollowObject(player,{200,windowSize.y*0.5f});

    level->generateLevel();
}

void ChaseAndImpactGame::initPlayerObject(string playerName, sre::SpriteAtlas* atlas, glm::vec2 startPosition,
	SDL_Keycode upKey, SDL_Keycode leftKey, SDL_Keycode rightKey) {

}

void ChaseAndImpactGame::update(float time) {
    updatePhysics();
	if (time > 0.03) // if framerate approx 30 fps then run two physics steps
	{
		updatePhysics();
	}
    for (int i=0;i<sceneObjects.size();i++){
        sceneObjects[i]->update(time);
    }
}

void ChaseAndImpactGame::render() {
    auto rp = RenderPass::create()
            .withCamera(camera->getCamera())
            .withClearColor(true, backgroundColor)
            .build();

    if (doDebugDraw){
        static Profiler profiler;
        profiler.update();
        profiler.gui(false);
    }

    auto pos = camera->getGameObject()->getPosition();

    auto spriteBatchBuilder = SpriteBatch::create();
    for (auto & go : sceneObjects){
        go->renderSprite(spriteBatchBuilder);
    }

    auto sb = spriteBatchBuilder.build();
    rp.draw(sb);

    if (doDebugDraw){
        world->DrawDebugData();
        rp.drawLines(debugDraw.getLines());
        debugDraw.clear();
    }
}

void ChaseAndImpactGame::onKey(SDL_Event &event) {
    for (auto & gameObject: sceneObjects) {
        for (auto & c : gameObject->getComponents()){
            bool consumed = c->onKey(event);
            if (consumed){
                return;
            }
        }
    }

    if (event.type == SDL_KEYDOWN){
        switch (event.key.keysym.sym){
            case SDLK_z:
                camera->setZoomMode(!camera->isZoomMode());
                break;
            case SDLK_d:
                // press 'd' for physics debug
                doDebugDraw = !doDebugDraw;
                if (doDebugDraw){
                    world->SetDebugDraw(&debugDraw);
                } else {
                    world->SetDebugDraw(nullptr);
                }
                break;
        }
    }
}

std::shared_ptr<GameObject> ChaseAndImpactGame::createGameObject() {
    auto obj = shared_ptr<GameObject>(new GameObject());
    sceneObjects.push_back(obj);
    return obj;
}

void ChaseAndImpactGame::updatePhysics() {

    const int positionIterations = 4;
    const int velocityIterations = 12;
    world->Step(timeStep, velocityIterations, positionIterations);

    for (auto phys : physicsComponentLookup){
        PhysicsComponent* physicsComponent = phys.second;
        if (physicsComponent->isAutoUpdate() == false) continue;
        auto position = physicsComponent->getBody()->GetPosition();
        float angle = physicsComponent->getBody()->GetAngle();
        auto gameObject = physicsComponent->getGameObject();
        gameObject->setPosition(glm::vec2(position.x*physicsScale, position.y*physicsScale));
        gameObject->setRotation(angle);
    }
}

void ChaseAndImpactGame::initPhysics() {
    float gravity = -9.8f; // 9.8 m/s2
    delete world;
    world = new b2World(b2Vec2(0,gravity));
    world->SetContactListener(this);

    if (doDebugDraw){
        world->SetDebugDraw(&debugDraw);
    }
}

void ChaseAndImpactGame::BeginContact(b2Contact *contact) {
    b2ContactListener::BeginContact(contact);
    handleContact(contact, true);
}

void ChaseAndImpactGame::EndContact(b2Contact *contact) {
    b2ContactListener::EndContact(contact);
    handleContact(contact, false);
}

void ChaseAndImpactGame::deregisterPhysicsComponent(PhysicsComponent *r) {
    auto iter = physicsComponentLookup.find(r->getFixture());
    if (iter != physicsComponentLookup.end()){
        physicsComponentLookup.erase(iter);
    } else {
        assert(false); // cannot find physics object
    }
}

void ChaseAndImpactGame::registerPhysicsComponent(PhysicsComponent *r) {
    physicsComponentLookup[r->getFixture()] = r;
}

void ChaseAndImpactGame::handleContact(b2Contact *contact, bool begin) {
    auto fixA = contact->GetFixtureA();
    auto fixB = contact->GetFixtureB();
    PhysicsComponent* physA = physicsComponentLookup[fixA];
    PhysicsComponent* physB = physicsComponentLookup[fixB];
    auto & aComponents = physA->getGameObject()->getComponents();
    auto & bComponents = physB->getGameObject()->getComponents();
    for (auto & c : aComponents){
        if (begin){
            c->onCollisionStart(physB);
        } else {
            c->onCollisionEnd(physB);
        }
    }
    for (auto & c : bComponents){
        if (begin){
            c->onCollisionStart(physA);
        } else {
            c->onCollisionEnd(physA);
        }
    }
}

