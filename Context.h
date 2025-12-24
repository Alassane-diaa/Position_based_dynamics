#ifndef CONTEXT_H
#define CONTEXT_H

#include <vector>

struct Vec2{
    float x;
    float y;
};

struct Particle{
    Vec2 pos;
    Vec2 predicted_pos;
    Vec2 velocity;
    float radius;
    float mass;
};

class Context
{
public:
    Context();
    ~Context() = default;
    void updatePhysicalSystem(float dt);
    std::vector<Particle>& getParticles() { return particles; };
    
    private:
    int particle_count;
    std::vector<Particle> particles;
    
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