#include <sre/Profiler.hpp>
#include <iostream>
#include <ctime>
#include <unordered_map>
#include "glm/gtx/transform.hpp"
#include "ChaseAndImpactGame.hpp"
#include "GameObject.hpp"
#include "sre/RenderPass.hpp"
#include "sre/Texture.hpp"
#include "SpriteComponent.hpp"
#include "Box2D/Dynamics/Contacts/b2Contact.h"
#include "PhysicsComponent.hpp"
#include "CharacterControllerComponent.hpp"
#include "BoulderMovementComponent.hpp"
#include "ParticleSystemComponent.hpp"
#include "NameplateComponent.h"
#include "PlatformComponent.hpp"

using namespace std;
using namespace sre;

const glm::vec2 ChaseAndImpactGame::windowSize(900, 440);

ChaseAndImpactGame* ChaseAndImpactGame::instance = nullptr;

ChaseAndImpactGame::ChaseAndImpactGame() :debugDraw(physicsScale)
{
	instance = this;
	r.setWindowSize(windowSize);

	time_t t;                   // random seed based on time
	srand((unsigned)time(&t));

	bool useVsync = true;
	r.init(SDL_INIT_EVERYTHING, SDL_WINDOW_OPENGL, useVsync);

	textures.push_back(Texture::getSphereTexture());
	textures.push_back(Texture::getWhiteTexture());

	//backgroundColor = glm::vec4(0.119, 0.778, 1.000, 0.8);
//	backgroundColor = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
//	backgroundColor = glm::vec4(0.054, 0.215, 0.207, 1);
	backgroundColor = glm::vec4(0.022, 0.214, 0.275, 1);

	spriteAtlas = SpriteAtlas::create("platformer-art-deluxe.json", "platformer-art-deluxe.png");

	level = std::shared_ptr<Level>(new Level(this, spriteAtlas));

	initPhysics();
	initLevel();

	// setup callback functions
	r.keyEvent = [&](SDL_Event& e) {
		onKey(e);
	};
	r.frameUpdate = [&](float deltaTime) {
		update(deltaTime);
	};
	r.frameRender = [&]() {
		render();
	};
	// start game loop
	r.startEventLoop();
}

void ChaseAndImpactGame::initLevel() {


	level->generateLevel();

	// Adding the boulder to the game object array AFTER the level is generated means that the boulder will be drawn on top
	// of the platforms instead of underneath them. 
	auto boulderObj = createGameObject();
	boulderObj->name = "Boulder";
	auto boulderSpriteComponent = boulderObj->addComponent<SpriteComponent>();
	auto boulder = spriteAtlas->get("boulder.png");
	boulder.setFlip({ false, false });
	boulder.setScale(glm::vec2(1.5f, 1.5f));
	boulderSpriteComponent->setSprite(boulder);
	boulderMovement = boulderObj->addComponent<BoulderMovementComponent>().get();
	boulderMovement->init(physicsScale, 195.0f, glm::vec2(0.0f, 7.1f), 1.0f);
	boulderMovement->CanMove = true;
	boulderMovement->setGameInstance(this);

	auto camObj = createGameObject();
	camObj->name = "Camera";
	camObj->setPosition(windowSize*0.5f);
	camera = camObj->addComponent<SideScrollingCamera>();
	camera->setFollowObject(boulderObj, { 450 ,windowSize.y*0.5f });

	initPlayerObject("Player 1", 19, glm::vec2{ 9.5, 2.5 }, ImVec4(255, 255, 255, 1.0f), glm::vec4(1.0f, 0.0f, 0.0f, 0.7f), SDL_Keycode(SDLK_w), SDL_Keycode(SDLK_a), SDL_Keycode(SDLK_d));
	initPlayerObject("Player 2", 19, glm::vec2{ 10.5, 2.5 }, ImVec4(255, 255, 255, 1.0f), glm::vec4(0.0f, 1.0f, 0.0f, 0.7f), SDL_Keycode(SDLK_UP), SDL_Keycode(SDLK_LEFT), SDL_Keycode(SDLK_RIGHT));

}

/*
* Used to initialize a player object.
* std::string playerName: String name of the Object.
* int spriteAtlastStartIndex: The Start Index of the sprite used.
* glm::vec2 startPosition: The start position of the player object (Level::tileSize already factored in).
* ImVec4 nameplateColor: Vector deciding the color of the text appearing over the players head.
* glm::vec4 particleColor: Vector used to determine the color the trail that follows the player.
* SDL_Keycode upKey: The key used to jump.
* SDL_Keycode leftKey: The key used to go left.
* SDL_Keycode rightKey: The key used to go right.
*/
void ChaseAndImpactGame::initPlayerObject(std::string playerName, int spriteAtlasStartIndex, glm::vec2 startPosition, ImVec4 nameplateColor, glm::vec4 particleColor,
	SDL_Keycode upKey, SDL_Keycode leftKey, SDL_Keycode rightKey) {
	auto player = createGameObject();
	player->name = playerName;
	player->position = startPosition * Level::tileSize;

	auto playerSprite = player->addComponent<SpriteComponent>();
	auto playerSpriteObj = spriteAtlas->get(std::to_string(spriteAtlasStartIndex) + ".png");
	playerSprite->setSprite(playerSpriteObj);


	auto characterController = player->addComponent<CharacterControllerComponent>();
	characterController->init(physicsScale, 10, b2BodyType::b2_dynamicBody, startPosition, 1.0f);

	auto particleSystem = player->addComponent<ParticleSystemComponent>();
	particleSystem->init(500, textures[0], particleColor);
	particleSystem->gravity = { 0,-.2,0 };
	particleSystem->lifeSpan = 2.0f;


	auto namePlate = player->addComponent<NameplateComponent>();
	const char* nameS = player->name.c_str();
	namePlate->init(nameS, windowSize.x, windowSize.y, 32.0f, nameplateColor, camera.get());

	characterController->setKeyCodes(upKey, leftKey, rightKey);

	characterController->setSprites(
		spriteAtlas->get(std::to_string(spriteAtlasStartIndex) + ".png"),
		spriteAtlas->get(std::to_string(spriteAtlasStartIndex + 1) + ".png"),
		spriteAtlas->get(std::to_string(spriteAtlasStartIndex + 2) + ".png"),
		spriteAtlas->get(std::to_string(spriteAtlasStartIndex + 3) + ".png"),
		spriteAtlas->get(std::to_string(spriteAtlasStartIndex + 4) + ".png"),
		spriteAtlas->get(std::to_string(spriteAtlasStartIndex + 5) + ".png")
	);


}

void ChaseAndImpactGame::update(float time) {

	updatePhysics();
	if (time > 0.03) // if framerate approx 30 fps then run two physics steps
	{
		updatePhysics();
	}

	for (auto & gameObj : sceneObjects) {
		gameObj.second->update(time);
	}

	destroyGameObjects();		//Only destroy objects at the end of frame, once all physics have been computed
}

void ChaseAndImpactGame::render() {
	auto rp = RenderPass::create()
		.withCamera(camera->getCamera())
		.withClearColor(true, backgroundColor)
		.build();

	if (doDebugDraw) {
		static Profiler profiler;
		profiler.update();
		profiler.gui(false);
	}

	auto pos = camera->getGameObject()->getPosition();

	auto spriteBatchBuilder = SpriteBatch::create();

	for (auto & pair : sceneObjects) {
		auto go = pair.second;
		go->renderSprite(spriteBatchBuilder);
		auto particle = go->getComponent<ParticleSystemComponent>();
		if (particle)
		{
			particle->draw(rp, glm::mat4(1));
		}
		auto namePlate = go->getComponent<NameplateComponent>();
		if (namePlate)
		{
			namePlate->render();
		}
	}

	auto sb = spriteBatchBuilder.build();
	rp.draw(sb);

	if (doDebugDraw) {
		world->DrawDebugData();
		rp.drawLines(debugDraw.getLines());
		debugDraw.clear();
	}

}

void ChaseAndImpactGame::onKey(SDL_Event &event) {
	for (auto & pair : sceneObjects) {
		auto gameObject = pair.second;
		for (auto & c : gameObject->getComponents()) {
			bool consumed = c->onKey(event);
			if (consumed) {
				return;
			}
		}
	}

	if (event.type == SDL_KEYDOWN) {
		switch (event.key.keysym.sym) {
		case SDLK_z:
			camera->setZoomMode(!camera->isZoomMode());		//This is pretty cool
			break;
		case SDLK_p:
			// press 'p' for physics debug
			doDebugDraw = !doDebugDraw;
			if (doDebugDraw) {
				world->SetDebugDraw(&debugDraw);
			}
			else {
				world->SetDebugDraw(nullptr);
			}
			break;
		case SDLK_ESCAPE:
			resetGame();
			break;
		}

	}
}

std::shared_ptr<GameObject> ChaseAndImpactGame::createGameObject() {
	shared_ptr<GameObject> obj = shared_ptr<GameObject>(new GameObject());
	sceneObjects.emplace(obj.get(), obj);
	return obj;
}

void ChaseAndImpactGame::destroyGameObject(GameObject* ptr)
{
	markedForDestroy.push_back(ptr);
}


void ChaseAndImpactGame::updatePhysics() {

	const int positionIterations = 4;
	const int velocityIterations = 12;
	world->Step(timeStep, velocityIterations, positionIterations);

	for (auto phys : physicsComponentLookup) {
		PhysicsComponent* physicsComponent = phys.second;
		if (physicsComponent->isAutoUpdate() == false) continue;
		auto position = physicsComponent->getBody()->GetPosition();
		float angle = physicsComponent->getBody()->GetAngle();
		auto gameObject = physicsComponent->getGameObject();
		gameObject->setPosition(glm::vec2(position.x*physicsScale, position.y*physicsScale));
		gameObject->setRotation(angle);
	}
}

void ChaseAndImpactGame::destroyGameObjects()
{
	if (markedForDestroy.size() > 0) {
		for (auto it = markedForDestroy.begin(); it != markedForDestroy.end(); it++) {
			auto gameObjectPair = sceneObjects.find(*it);

			if (gameObjectPair != sceneObjects.end()) {

				sceneObjects.erase(gameObjectPair);
			}
		}

		markedForDestroy.clear();
	}
}

void ChaseAndImpactGame::resetGame()
{
	level->resetLevel();
	markedForDestroy.clear();
	sceneObjects.clear();

	initLevel();
}

void ChaseAndImpactGame::initPhysics() {
	float gravity = -9.8f; // 9.8 m/s2
	delete world;
	world = new b2World(b2Vec2(0, gravity));
	world->SetContactListener(this);

	if (doDebugDraw) {
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
	if (iter != physicsComponentLookup.end()) {
		physicsComponentLookup.erase(iter);
	}
	else {
		assert(false); // cannot find physics object
	}
}

void ChaseAndImpactGame::registerPhysicsComponent(PhysicsComponent *r) {
	physicsComponentLookup[r->getFixture()] = r;
}

void ChaseAndImpactGame::handleContact(b2Contact *contact, bool begin) {
	auto fixA = contact->GetFixtureA();
	auto fixB = contact->GetFixtureB();

	auto physA = physicsComponentLookup.find(fixA);
	auto physB = physicsComponentLookup.find(fixB);
	if (physA != physicsComponentLookup.end() && physB != physicsComponentLookup.end()) {
		auto & aComponents = physA->second->getGameObject()->getComponents();
		auto & bComponents = physB->second->getGameObject()->getComponents();
		for (auto & c : aComponents) {
			if (begin) {
				c->onCollisionStart(physB->second);
			}
			else {
				c->onCollisionEnd(physB->second);
			}
		}
		for (auto & c : bComponents) {
			if (begin) {
				c->onCollisionStart(physA->second);
			}
			else {
				c->onCollisionEnd(physA->second);
			}
		}
	}
}

void ChaseAndImpactGame::endGame(std::string loser) {

	// TODO Create win screen, slow down gameplay (maybe), activate spacebar to restart the game

}

float ChaseAndImpactGame::getCurrentUpdatePosition()
{
	return level->getCurrentUpdatePosition();
}

void ChaseAndImpactGame::updateLevel()
{
	level->updateLevel();
}

