#include <sre/Profiler.hpp>
#include <iostream>
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

using namespace std;
using namespace sre;

const glm::vec2 ChaseAndImpactGame::windowSize(800, 600);

ChaseAndImpactGame* ChaseAndImpactGame::instance = nullptr;

ChaseAndImpactGame::ChaseAndImpactGame()
	:debugDraw(physicsScale)
{
	instance = this;
	r.setWindowSize(windowSize);
	bool useVsync = true;
	r.init(SDL_INIT_EVERYTHING, SDL_WINDOW_OPENGL, useVsync);

	textures.push_back(Texture::getSphereTexture());
	textures.push_back(Texture::getWhiteTexture());

	backgroundColor = glm::vec4(0.6, 0.6, 1, 1);

	spriteAtlas = SpriteAtlas::create("platformer-art-deluxe.json", "platformer-art-deluxe.png");

	level = Level::createDefaultLevel(this, spriteAtlas);

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
	initPhysics();

	auto camObj = createGameObject();
	camObj->name = "Camera";
	camera = camObj->addComponent<SideScrollingCamera>();
	camObj->setPosition(windowSize*0.5f);
	level->generateLevel();

	auto boulderObj = createGameObject();
	boulderObj->name = "Boulder";
	auto boulderSpriteComponent = boulderObj->addComponent<SpriteComponent>();
	auto boulder = spriteAtlas->get("boulder.png");
	boulder.setFlip({ false, false });
	boulderSpriteComponent->setSprite(boulder);
	boulderMovement = boulderObj->addComponent<BoulderMovementComponent>().get();
	boulderMovement->CanMove = true;

	auto camObj = createGameObject();
	camObj->name = "Camera";
	camera = camObj->addComponent<SideScrollingCamera>();
	camObj->setPosition(windowSize*0.5f);
	camera->setFollowObject(boulderObj, { 200,windowSize.y*0.5f });
	mainCam = camera;

	initPlayerObject("Player 1", 19, glm::vec2{ 2.5, 2.5 },
		SDL_Keycode(SDLK_w), SDL_Keycode(SDLK_a), SDL_Keycode(SDLK_d));
	initPlayerObject("Player 2", 19, glm::vec2{ 10.5, 2.5 },
		SDL_Keycode(SDLK_UP), SDL_Keycode(SDLK_LEFT), SDL_Keycode(SDLK_RIGHT));

	level->generateLevel();
}

/*
* Used to initialize a player object.
* std::string playerName: String name of the Object.
* int spriteAtlastStartIndex: The Start Index of the sprite used.
* glm::vec2 startPosition: The start position of the player object (Level::tileSize already factored in).
* SDL_Keycode upKey: The key used to jump.
* SDL_Keycode leftKey: The key used to go left.
* SDL_Keycode rightKey: The key used to go right.
*/
void ChaseAndImpactGame::initPlayerObject(std::string playerName, int spriteAtlasStartIndex, glm::vec2 startPosition,
	SDL_Keycode upKey, SDL_Keycode leftKey, SDL_Keycode rightKey) {
	auto player = createGameObject();
	player->name = playerName;
	auto playerSprite = player->addComponent<SpriteComponent>();
	auto playerSpriteObj = spriteAtlas->get(std::to_string(spriteAtlasStartIndex) + ".png");
	player->position = startPosition*Level::tileSize; 
	playerSprite->setSprite(playerSpriteObj);
	auto characterController = player->addComponent<CharacterControllerComponent>();
	characterController->init(physicsScale, 10, b2BodyType::b2_dynamicBody, startPosition, 1.0f);
	auto particleSystem = player->addComponent<ParticleSystemComponent>();
	particleSystem->init(500, textures[0]);
	particleSystem->gravity = { 0,-.2,0 };
	particleSystem->lifeSpan = 2.0f;
	//auto namePlate = player->addComponent<NameplateComponent>();
	//namePlate->init(playerName.c_str(), windowSize.x, windowSize.y, mainCam.get());
	
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
	for (int i = 0; i < sceneObjects.size(); i++) {
		sceneObjects[i]->update(time);
	}
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
	for (auto & go : sceneObjects) {
		go->renderSprite(spriteBatchBuilder);
		auto particle = go->getComponent<ParticleSystemComponent>();
		if (particle)
		{
			particle->draw(rp, glm::mat4(1));
		}
		//TODO Ask teacher for help with this. Doesn't currently work right.
		//auto namePlate = go->getComponent<NameplateComponent>();
		//if (namePlate)
		//{
		//	namePlate->render();
		//}
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
	for (auto & gameObject : sceneObjects) {
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
			camera->setZoomMode(!camera->isZoomMode());
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
	PhysicsComponent* physA = physicsComponentLookup[fixA];
	PhysicsComponent* physB = physicsComponentLookup[fixB];
	auto & aComponents = physA->getGameObject()->getComponents();
	auto & bComponents = physB->getGameObject()->getComponents();
	for (auto & c : aComponents) {
		if (begin) {
			c->onCollisionStart(physB);
		}
		else {
			c->onCollisionEnd(physB);
		}
	}
	for (auto & c : bComponents) {
		if (begin) {
			c->onCollisionStart(physA);
		}
		else {
			c->onCollisionEnd(physA);
		}
	}
}

