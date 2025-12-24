#include "Context.h"
#include <cmath>

using force = Vec2;

Context::Context() 
    : particle_count(0)
    , particles()
{}

void Context::updatePhysicalSystem(float dt)
{
    applyExternalForce(dt);
    dampVelocities(dt);
    updateExpectedPosition(dt);
    addStaticContactConstraints(dt);
    addDynamicContactConstraints(dt);
    applyFriction(dt);
    projectConstraints();
    updateVelocityAndPosition(dt);
    deleteContactConstraints();
}

void Context::applyExternalForce(float dt)
{
    // Pour l'instant j'ai que la gravité
    force gravity = {0.0f, -2000.0f};

    for (auto& particle : particles) {
        particle.velocity.y += gravity.y * dt;
    }
}

void Context::updateExpectedPosition(float dt)
{
    for (auto& particle : particles) {
        particle.predicted_pos.x = particle.pos.x + particle.velocity.x * dt;
        particle.predicted_pos.y = particle.pos.y + particle.velocity.y * dt;
    }    
}

void Context::dampVelocities(float dt)
{
    // On prend un facteur d'amortissement exponentiel
    const float damping = 3.0f; 
    float factor = std::exp(-damping * dt);
    for (auto& p : particles) {
        p.velocity.x *= factor;
        p.velocity.y *= factor;
    }
}


void Context::addDynamicContactConstraints(float dt)
{
    const float restitution = 0.2f; // coef de restitution
    const float groundHeight = -280.0f;
    for (auto& p : particles) {
        if (p.predicted_pos.y < groundHeight + p.radius) {
            p.predicted_pos.y = groundHeight + p.radius;
            if (p.velocity.y < 0.0f) {
                p.velocity.y = -p.velocity.y * restitution;
            }
        }
    }
}

void Context::addStaticContactConstraints(float dt)
{
    const float groundHeight = -280.0f;
    for (Particle &p : particles) {
        if (p.predicted_pos.y < (groundHeight + p.radius)) {
            p.predicted_pos.y = groundHeight + p.radius;
        }
    }
}

void Context::projectConstraints()
{
    for (size_t i = 0; i < particles.size(); ++i) {
        for (size_t j = i + 1; j < particles.size(); ++j) {
            
            Particle &p1 = particles[i];
            Particle &p2 = particles[j];
            Vec2 collisionVector;
            collisionVector.x = p1.predicted_pos.x - p2.predicted_pos.x;
            collisionVector.y = p1.predicted_pos.y - p2.predicted_pos.y;
            
            float distance = sqrt(collisionVector.x * collisionVector.x + collisionVector.y * collisionVector.y);
            float minDistance = p1.radius + p2.radius;

            if (distance < minDistance) {
                if (distance < 0.0001f) { 
                    collisionVector = Vec2{0.0f, 10.0f}; 
                    distance = 0.0001f;
                }

                Vec2 n;
                n.x = collisionVector.x / distance;
                n.y = collisionVector.y / distance;

                float C = distance - minDistance;

                float w1 = 1/p1.mass;
                float w2 = 1/p2.mass;
                float wTotal = w1 + w2;
                
                if (wTotal == 0) continue;

                float sigma1 = (w1 / wTotal) * C;
                float sigma2 = (w2 / wTotal) * C;

                p1.predicted_pos.x -= n.x * sigma1;
                p1.predicted_pos.y -= n.y * sigma1;
                
                p2.predicted_pos.x += n.x * sigma2;
                p2.predicted_pos.y += n.y * sigma2;
            }
        }
    }
}

void Context::applyFriction(float dt)
{
    const float groundHeight = -280.0f;
    const float mu = 8.0f; 
    for (auto& p : particles) {
        bool onGround = (p.predicted_pos.y <= groundHeight + p.radius + 0.5f);
        if (onGround) {
            float frictionFactor = std::max(0.0f, 1.0f - mu * dt);
            p.velocity.x *= frictionFactor;
        }
    }
}

void Context::deleteContactConstraints()
{
    // Pas compris, à faire après
}

void Context::updateVelocityAndPosition(float dt)
{
    for (auto& p : particles) {
        p.velocity.x = (p.predicted_pos.x - p.pos.x) / dt;
        p.velocity.y = (p.predicted_pos.y - p.pos.y) / dt;
        p.pos = p.predicted_pos;
    }
}