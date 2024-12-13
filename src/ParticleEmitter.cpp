#include "ParticleEmitter.h"
#include <glm/gtc/random.hpp>
#include <algorithm>
#include <random>

//Refrences for Particle Emitter: https://learnopengl.com/In-Practice/2D-Game/Particles

ParticleEmitter::ParticleEmitter(int maxParticles, EmitterType type)
: maxParticles(maxParticles), emitterType(type)
{
    particles.resize(maxParticles);

    glGenVertexArrays(1,&VAO);
    glBindVertexArray(VAO);

    glGenBuffers(1,&VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, maxParticles*3*sizeof(float), nullptr, GL_DYNAMIC_DRAW);

    glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,0,(void*)0);
    glEnableVertexAttribArray(0);

    glBindVertexArray(0);

    for (auto &p : particles) {
        p.life = -1.0f;
    }

    emissionPosition = glm::vec3(0.0f);
}

void ParticleEmitter::update(float dt) {
    for (auto &p : particles) {
        if (p.life > 0.0f) {
            p.life -= dt;

            if (p.life > 0.0f) {
                p.position += p.velocity * dt;
                // Add upward acceleration for flame
                p.velocity += glm::vec3(0.0f, 0.5f * dt, 0.0f);

                // Slight horizontal drift
                float drift = 0.1f;
                p.position.x += (glm::linearRand(-drift, drift) * dt);
                p.position.z += (glm::linearRand(-drift, drift) * dt);
            } else {
                p.life = -1.0f;
            }
        }
    }
}

void ParticleEmitter::emit(int count) {
    for (int i = 0; i < count; i++) {
        // Find a dead particle
        auto it = std::find_if(particles.begin(), particles.end(), [](const Particle &p){return p.life < 0.0f;});
        if (it != particles.end()) {
            // Adjust parameters based on emitter type
            float radius, upSpeedMin, upSpeedMax, lifeSpan, sizeVal;
            if (emitterType == EmitterType::CoreFlame) {
                // Core flame: small, bright, short-lived, fast upward
                radius = 0.01f;
                upSpeedMin = 1.0f; upSpeedMax = 1.5f;
                lifeSpan = 0.7f;
                sizeVal = 15.0f;
            } else {
                // Heat haze: slightly larger, more subtle, longer life but more transparent
                radius = 0.05f;
                upSpeedMin = 0.5f; upSpeedMax = 1.0f;
                lifeSpan = 1.5f;
                sizeVal = 20.0f;
            }

            float xOffset = glm::linearRand(-radius, radius);
            float zOffset = glm::linearRand(-radius, radius);
            it->position = emissionPosition + glm::vec3(xOffset, 0.02f, zOffset);

            float upSpeed = glm::linearRand(upSpeedMin, upSpeedMax);
            float horzSpread = 0.05f;
            it->velocity = glm::vec3(glm::linearRand(-horzSpread, horzSpread),
                                     upSpeed,
                                     glm::linearRand(-horzSpread, horzSpread));

            it->life = lifeSpan; 
            it->size = sizeVal;
        }
    }
}

void ParticleEmitter::draw() {
    std::vector<float> positions;
    for (auto &p : particles) {
        if (p.life > 0.0f) {
            positions.push_back(p.position.x);
            positions.push_back(p.position.y);
            positions.push_back(p.position.z);
        }
    }

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferSubData(GL_ARRAY_BUFFER,0,positions.size()*sizeof(float),positions.data());
    glDrawArrays(GL_POINTS,0,(GLsizei)(positions.size()/3));
    glBindVertexArray(0);
}
