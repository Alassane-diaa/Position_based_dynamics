#ifndef CONTEXT_H
#define CONTEXT_H

#include <vector>
#include <iostream>
#include "MyCollider.h"


struct ParticleLink {
    int particleA;
    int particleB; 
    float restLength;
    float stiffness;    
};


class Context
{
public:
    Context();
    ~Context();
    void updatePhysicalSystem(float dt);
    std::vector<Particle>& getParticles() { return particles; };
    std::vector<MyCollider*>& getColliders() { return colliders; };
    std::vector<ParticleLink>& getParticleLinks() { return particleLinks; };
    
    void addLinkedStructure(const Vec2& center, float size, float particleRadius, float particleMass, float linkStiffness = 0.8f);
    
protected:
    int particle_count;
    std::vector<Particle> particles;
    std::vector<ParticleLink> particleLinks;
    std::vector<MyCollider*> colliders;
    void applyExternalForce(float dt);
    void dampVelocities(float dt);
    void updateExpectedPosition(float dt);
    void addDynamicContactConstraints(float dt);
    void addStaticContactConstraints(float dt);
    void projectParticleLinks(); 
    void applyFriction(float dt);
    void updateVelocityAndPosition(float dt);

};

#endif // CONTEXT_H