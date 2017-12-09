#pragma once

#include <Box2D/Box2D.h>
#include "Component.hpp"
#include "SpriteComponent.hpp"
#include "sre/Sprite.hpp"

class CharacterControllerComponent : public Component, public b2RayCastCallback {
public:
    explicit CharacterControllerComponent(GameObject *gameObject);

	void init(float physicsScale, float radius, b2BodyType bodyType, glm::vec2 center, float density);

	void setKeyCodes(SDL_Keycode up, SDL_Keycode left, SDL_Keycode right);

    void setSprites(sre::Sprite standing,
                    sre::Sprite walk1,
                    sre::Sprite walk2,
                    sre::Sprite flyUp,
                    sre::Sprite fly,
                    sre::Sprite flyDown);

    void updateSprite(float deltaTime);

    void update(float deltaTime) override;

    bool onKey(SDL_Event &event) override;

    void jump();

    // raycast callback
    virtual float32 ReportFixture(	b2Fixture* fixture, const b2Vec2& point,
                                      const b2Vec2& normal, float32 fraction) override;

    void onCollisionStart(PhysicsComponent *comp) override;

    void onCollisionEnd(PhysicsComponent *comp) override;
private:
    sre::Sprite standing;
    sre::Sprite walk1;
    sre::Sprite walk2;
    sre::Sprite flyUp;
    sre::Sprite fly;
    sre::Sprite flyDown;

	SDL_Keycode UPKEY;
	SDL_Keycode LEFTKEY;
	SDL_Keycode RIGHTKEY;

    std::shared_ptr<SpriteComponent> spriteComponent;
    std::shared_ptr<PhysicsComponent> characterPhysics;
    bool isGrounded = false;
    float radius;
	float animationSpeed;
	float animationDeltaTime = 0;
	float maximumVelocity = 2.7f;
	float jumpForce = 0.15f;
    bool left = false;
    bool right = false;
	bool direction = true;

};
