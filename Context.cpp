#include "Context.h"
#include "MyCollider.h"
#include <cmath>

using force = Vec2;

Context::Context() 
    : particle_count(0)
    , particles()
    , particleLinks()
    , colliders()
{}

Context::~Context()
{
    for (MyCollider* collider : colliders) {
        delete collider;
    }
    colliders.clear();
}

void Context::updatePhysicalSystem(float dt)
{
    applyExternalForce(dt);
    dampVelocities(dt);
    updateExpectedPosition(dt);
    addStaticContactConstraints(dt);
    addDynamicContactConstraints(dt);
    projectParticleLinks(); 
    applyFriction(dt);
    updateVelocityAndPosition(dt);
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
    const float damping = 1.0f; 
    float factor = std::exp(-damping * dt);
    for (auto& p : particles) {
        p.velocity.x *= factor;
        p.velocity.y *= factor;
    }
}

void Context::addDynamicContactConstraints(float dt)
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

void Context::addStaticContactConstraints(float dt)
{
    for (MyCollider* collider : colliders) {
        for (Particle &p : particles) {
            if (collider->checkCollision(p)) {
                collider->solveCollision(p);
            }
        }
    }
}

void Context::projectParticleLinks()
{
    for (ParticleLink& link : particleLinks) {
        if (link.particleA < 0 || link.particleA >= (int)particles.size() ||
            link.particleB < 0 || link.particleB >= (int)particles.size()) {
            continue;
        }
        
        Particle& p1 = particles[link.particleA];
        Particle& p2 = particles[link.particleB];
        
        Vec2 delta;
        delta.x = p2.predicted_pos.x - p1.predicted_pos.x;
        delta.y = p2.predicted_pos.y - p1.predicted_pos.y;
        
        float currentLength = std::sqrt(delta.x * delta.x + delta.y * delta.y);
        
        if (currentLength < 0.0001f) continue;
        
        float diff = (currentLength - link.restLength) / currentLength;
        
        float w1 = 1.0f / p1.mass;
        float w2 = 1.0f / p2.mass;
        float wTotal = w1 + w2;
        
        if (wTotal == 0) continue;
        
        float correction = diff * link.stiffness;
        
        p1.predicted_pos.x += delta.x * correction * (w1 / wTotal);
        p1.predicted_pos.y += delta.y * correction * (w1 / wTotal);
        
        p2.predicted_pos.x -= delta.x * correction * (w2 / wTotal);
        p2.predicted_pos.y -= delta.y * correction * (w2 / wTotal);
   }
}

void Context::addLinkedStructure(const Vec2& center, float size, float particleRadius, float particleMass, float linkStiffness)
{
    float halfSize = size / 2.0f;
    
    int startIndex = particles.size();
    
    Vec2 positions[4] = {
        {center.x - halfSize, center.y + halfSize},
        {center.x + halfSize, center.y + halfSize},
        {center.x + halfSize, center.y - halfSize},
        {center.x - halfSize, center.y - halfSize}
    };
    
    for (int i = 0; i < 4; ++i) {
        particles.push_back(Particle{
            positions[i],
            positions[i],
            Vec2{0.0f, 0.0f},
            particleRadius,
            particleMass
        });
    }

    particleLinks.push_back(ParticleLink{startIndex + 0, startIndex + 1, size, linkStiffness});
    particleLinks.push_back(ParticleLink{startIndex + 1, startIndex + 2, size, linkStiffness});
    particleLinks.push_back(ParticleLink{startIndex + 2, startIndex + 3, size, linkStiffness});
    particleLinks.push_back(ParticleLink{startIndex + 3, startIndex + 0, size, linkStiffness});
    
    float diagonalLength = size * std::sqrt(2.0f);
    particleLinks.push_back(ParticleLink{startIndex + 0, startIndex + 2, diagonalLength, linkStiffness});
    particleLinks.push_back(ParticleLink{startIndex + 1, startIndex + 3, diagonalLength, linkStiffness});
}

void Context::applyFriction(float dt)
{
    const float mu = 10.0f; 
    for (MyCollider* collider : colliders) {
        for (Particle &p : particles) {
            if (collider->checkCollision(p)) {
                if (const PlanCollider* plane = dynamic_cast<const PlanCollider*>(collider)) {
                    Vec2 planeNormal = plane->getPlaneNormal();
                    Vec2 tangent;
                    tangent.x = -planeNormal.y;
                    tangent.y = planeNormal.x;

                    float v_n = p.velocity.x * planeNormal.x + p.velocity.y * planeNormal.y;
                    float v_t = p.velocity.x * tangent.x + p.velocity.y * tangent.y;

                    float frictionImpulse = mu * std::abs(v_n) * dt;

                    if (std::abs(v_t) <= frictionImpulse) {
                        p.velocity.x -= v_t * tangent.x;
                        p.velocity.y -= v_t * tangent.y;
                    } else {
                        float sign = (v_t > 0) ? 1.0f : -1.0f;
                        p.velocity.x -= sign * frictionImpulse * tangent.x;
                        p.velocity.y -= sign * frictionImpulse * tangent.y;
                    }
                }
            }
        }
    }
}

void Context::updateVelocityAndPosition(float dt)
{
    for (auto& p : particles) {
        p.velocity.x = (p.predicted_pos.x - p.pos.x) / dt;
        p.velocity.y = (p.predicted_pos.y - p.pos.y) / dt;
        p.pos = p.predicted_pos;
    }
}