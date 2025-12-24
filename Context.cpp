#include "Context.h"
#include <cmath>

using force = Vec2;

Context::Context() 
    : particle_count(0)
    , particles()
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
    applyFriction(dt);
    projectConstraints();
    deleteContactConstraints();
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
    const float damping = 3.0f; 
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
                if (const SphereCollider* sphere = dynamic_cast<const SphereCollider*>(collider)) {
                    Vec2 p_to_center;
                    p_to_center.x = p.predicted_pos.x - sphere->getSphereCenter().x;
                    p_to_center.y = p.predicted_pos.y - sphere->getSphereCenter().y;
                    float distance = sqrt(p_to_center.x * p_to_center.x + p_to_center.y * p_to_center.y);
                    float minDistance = p.radius + sphere->getSphereRadius();

                    if (distance < 0.0001f) { 
                        p_to_center = Vec2{0.0f, 10.0f}; 
                        distance = 0.0001f;
                    }

                    Vec2 n;
                    n.x = p_to_center.x / distance;
                    n.y = p_to_center.y / distance;

                    p.predicted_pos.x = sphere->getSphereCenter().x + n.x * minDistance;
                    p.predicted_pos.y = sphere->getSphereCenter().y + n.y * minDistance;
                } else if (const PlanCollider* plane = dynamic_cast<const PlanCollider*>(collider)) {
                    Vec2 planePoint = plane->getPointOnPlane();
                    Vec2 planeNormal = plane->getPlaneNormal();

                    Vec2 p_to_plane;
                    p_to_plane.x = p.predicted_pos.x - planePoint.x;
                    p_to_plane.y = p.predicted_pos.y - planePoint.y;

                    float distance = p_to_plane.x * planeNormal.x + p_to_plane.y * planeNormal.y;
                    Vec2 q_c;
                    q_c.x = p.predicted_pos.x - distance * planeNormal.x;
                    q_c.y = p.predicted_pos.y - distance * planeNormal.y;
                    
                    Vec2 p_moins_qc;
                    p_moins_qc.x = p.predicted_pos.x - q_c.x;
                    p_moins_qc.y = p.predicted_pos.y - q_c.y;

                    float C = (p_moins_qc.x * planeNormal.x + p_moins_qc.y * planeNormal.y) - p.radius;

                    p.predicted_pos.x -= planeNormal.x * C;
                    p.predicted_pos.y -= planeNormal.y * C;
                }    
            }
        }
    }
}

void Context::projectConstraints()
{
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