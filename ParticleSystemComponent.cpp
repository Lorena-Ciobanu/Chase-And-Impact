//
// Created by Morten Nobel-Jørgensen on 11/21/17.
//

#include "ParticleSystemComponent.hpp"
#include "GameObject.hpp"

ParticleSystemComponent::ParticleSystemComponent(GameObject *gameObject) : Component(gameObject)
{
}

void ParticleSystemComponent::init(int particleCount, std::shared_ptr<sre::Texture> texture, glm::vec4 particleColor)
{
	particles.resize(particleCount);
	material = sre::Shader::getStandardParticles()->createMaterial();
	material->setTexture(texture);
	positions.resize(particleCount, glm::vec3(0, 0, 0));
	colors.resize(particleCount, particleColor);
	sizes.resize(particleCount, 0);
	uvs.resize(particleCount, glm::vec4{ 0.0f,0.0f,1.0f,1.0f });

	emitter = [&](Particle& p) {
		p.position = glm::vec3(getGameObject()->getPosition(), 0);
		p.velocity = glm::vec3(0);//glm::sphericalRand(1.0f);
		p.rotation = 0;
		p.angularVelocity = 0;
	};

	colorInterpolation = [&](const Particle& p) {
		return glm::mix(particleColor, particleColor, 1.0f);
	};

	sizeInterpolation = [](const Particle& p) {
		return 50;
	};

	// preallocate arrays for mesh data
	for (int i = 0; i < particleCount; i++) {
		positions[i] = glm::vec3(i*0.1f, i*0.1f, i*0.1f);
		colors[i] = particleColor;
		sizes[i] = 100;
		particles[i].index = i;
	}

	mesh = sre::Mesh::create()
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
			p.velocity += deltaTime * gravity;
			p.position += p.velocity * deltaTime;
			p.rotation += p.angularVelocity * deltaTime;
			p.alive = p.timeOfBirth + lifeSpan > time;
			if (p.alive)
				activeParticles++;
			p.normalizedAge = (time - p.timeOfBirth) / lifeSpan;
		}
	}
}

void ParticleSystemComponent::draw(sre::RenderPass& pr, glm::mat4 transform) {
	if (!visible) return;
	for (int i = 0; i < particles.size(); i++) {
		auto &p = particles[i];
		sizes[i] = p.alive ? sizeInterpolation(p) : 0;
		positions[i] = p.position;
		//colors[i] = colorInterpolation(p);
		uvs[i].w = p.rotation;
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
	particleIndex = (particleIndex + 1) % particles.size();
}

