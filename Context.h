#ifndef CONTEXT_H
#define CONTEXT_H

#include <vector>
#include <iostream>
#include "MyCollider.h"

struct StaticConstraint{
    MyCollider* collider;
    Particle& particle;
};


class Context
{
public:
    Context();
    ~Context();
    void updatePhysicalSystem(float dt);
    std::vector<Particle>& getParticles() { return particles; };
    std::vector<StaticConstraint>& getStaticConstraints() { return staticConstraints; };
    std::vector<MyCollider*>& getColliders() { return colliders; };
    
protected:
    int particle_count;
    std::vector<Particle> particles;
    std::vector<StaticConstraint> staticConstraints;
    std::vector<MyCollider*> colliders;
    void applyExternalForce(float dt);
    void dampVelocities(float dt);
    void updateExpectedPosition(float dt);
    void addDynamicContactConstraints(float dt);
    void addStaticContactConstraints(float dt);
    void projectConstraints();
    void applyFriction(float dt);
    void deleteContactConstraints();
    void updateVelocityAndPosition(float dt);

};

#endif // CONTEXT_H