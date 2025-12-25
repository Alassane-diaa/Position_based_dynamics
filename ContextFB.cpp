#include "ContextFB.h"

void ContextFB::updatePhysicalSystem(float dt)
{
    applyExternalForce(dt);
    dampVelocities(dt);
    updateExpectedPosition(dt);
    addStaticContactConstraints(dt);
    addDynamicContactConstraints(dt);
    applyFriction(dt);
    projectConstraints();
    deleteContactConstraints();
    updateVelocityAndPosition(dt);
    movePipes(dt); 
}

void ContextFB::addStaticContactConstraints(float dt)
{
    Context::addStaticContactConstraints(dt);
    for (MyCollider* collider : colliders) {
        for (Particle &p : particles) {
            if (collider->checkCollision(p)) {
                if (PipeCollider* pipe = dynamic_cast<PipeCollider*>(collider)) {
                    float half_width = PipeCollider::width / 2.0f;
                    float pipeX = pipe->getPosX();
                    float gapY = pipe->getGapY();
                    float gapSize = pipe->getGapSize();
                    float gapTop = gapY + gapSize / 2.0f;
                    float gapBottom = gapY - gapSize / 2.0f;
                    
                    if (p.predicted_pos.y > gapY) {
                        p.predicted_pos.y = gapTop - p.radius;
                        p.velocity.y = std::min(p.velocity.y, 0.0f);
                    } else {
                        p.predicted_pos.y = gapBottom + p.radius;
                        p.velocity.y = std::max(p.velocity.y, 0.0f);
                    }
                }
            }
        }
    }
}

void ContextFB::movePipes(float dt)
{
    const float speed = 100.0f; 
    for (MyCollider* collider : colliders) {
        if (PipeCollider* pipe = dynamic_cast<PipeCollider*>(collider)) {
            pipe->getPosX() -= speed * dt;
            if (pipe->getPosX() < -350.0f) { 
                pipe->getPosX() = 350.0f; 
            }
        }
    }
}