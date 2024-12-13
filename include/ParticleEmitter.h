#ifndef PARTICLE_EMITTER_H
#define PARTICLE_EMITTER_H

#include <GL/glew.h>
#include <vector>
#include <glm/glm.hpp>

struct Particle {
    glm::vec3 position;
    glm::vec3 velocity;
    float life;
    float size;
};

enum class EmitterType {
    CoreFlame,
    HeatHaze
};

class ParticleEmitter {
public:
    ParticleEmitter(int maxParticles, EmitterType type);

    void update(float dt);
    void draw();
    void emit(int count);
    void setEmissionPosition(const glm::vec3 &pos) { emissionPosition = pos; }

private:
    std::vector<Particle> particles;
    int maxParticles;

    GLuint VAO, VBO;
    glm::vec3 emissionPosition;
    EmitterType emitterType;
};

#endif
