//
// Created by Morten Nobel-Jørgensen on 11/21/17.
//

#include "ParticleSystemComponent.hpp"
#include "GameObject.hpp"

ParticleSystemComponent::ParticleSystemComponent(GameObject *gameObject) : Component(gameObject)
{
}

void ParticleSystemComponent::init(int particleCount, std::shared_ptr<sre::Texture> texture)
{
	positions.resize(particleCount, glm::vec3(0, 0, 0));
	colors.resize(particleCount, glm::vec4(1, 1, 1, 1));
	sizes.resize(particleCount, 0);
	uvs.resize(particleCount, glm::vec4{ 0.0f,0.0f,1.0f,1.0f });
	
	for (int i=0;i<particleCount;i++){
		particles.emplace_back(positions[i], colors[i], sizes[i], uvs[i]);
	}

    material = sre::Shader::getStandardParticles()->createMaterial();
    material->setTexture(texture);

	emitter = [&](Particle& p) {
		p.position = glm::vec3(getGameObject()->getPosition(),0);
		p.velocity = glm::vec3(0);//glm::sphericalRand(1.0f);
		p.rotation = 0;
		p.angularVelocity = 0;
	};

    updateAppearance = [](Particle& p) {
    };

    updatePhysics = [&](Particle& p, float deltaTime){
        p.velocity += deltaTime * gravity;
        p.position += p.velocity * deltaTime;
        p.rotation += p.angularVelocity * deltaTime;
    };

    // preallocate arrays for mesh data
    for (int i=0;i<particleCount;i++){
        positions[i] = glm::vec3(i*0.1f,i*0.1f,i*0.1f);
        colors[i] = glm::vec4(1,1,1,1);
        sizes[i] = 100;
        particles[i].setIndex(i);
    }

    mesh = sre::Mesh::create ()
            .withPositions(positions)
            .withColors(colors)
            .withParticleSizes(sizes)
            .withUVs(uvs)
            .withMeshTopology(sre::MeshTopology::Points)
            .build();
}

void ParticleSystemComponent::update(float deltaTime) {
	if (emitting && running) {
		int oldEmissions = static_cast<int>(emissions);
		emissions += deltaTime * emissionRate;
		int newEmissions = static_cast<int>(emissions);

		// emit number of particles
		for (int i = oldEmissions; i < newEmissions; i++) {
			emit();
		}
	}

	if (running) {
        time += deltaTime;
        activeParticles = 0;
        for (auto &p : particles) {
            updatePhysics(p, deltaTime);
            p.alive = p.timeOfBirth + lifeSpan > time;
            if (p.alive){
                activeParticles++;
                p.normalizedAge = (time - p.timeOfBirth) / lifeSpan;
            } else {
                p.size = 0;
            }
        }
    }
}

void ParticleSystemComponent::draw(sre::RenderPass& pr, glm::mat4 transform) {
	if (!visible) return;
    for (auto & p : particles){
        if (p.alive){
            updateAppearance(p);
        }
    }
    mesh->update()
            .withPositions(positions)
            .withColors(colors)
            .withParticleSizes(sizes)
            .withUVs(uvs)
            .build();
    pr.draw(mesh, transform, material);
}

int ParticleSystemComponent::getActiveParticles() {
	return activeParticles;
}

void ParticleSystemComponent::emit() {
    auto & p = particles[particleIndex];
    p.alive = true;
    p.timeOfBirth = time;
    p.normalizedAge = 0;
    emitter(p);
    particleIndex = static_cast<int>((particleIndex + 1) % particles.size());
}

