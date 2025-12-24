#ifndef CONTEXT_H
#define CONTEXT_H

#include <vector>

struct Vec2{
    float x;
    float y;
};

using Point = Vec2;

struct Particle{
    Vec2 pos;
    Vec2 predicted_pos;
    Vec2 velocity;
    float radius;
    float mass;
};

struct StaticConstraint{
    int particleIndex;
    Vec2 contactPoint;
    Vec2 normal;
};

class MyCollider;

class Context
{
public:
    Context();
    ~Context();
    void updatePhysicalSystem(float dt);
    std::vector<Particle>& getParticles() { return particles; };
    std::vector<StaticConstraint>& getStaticConstraints() { return staticConstraints; };
    std::vector<MyCollider*>& getColliders() { return colliders; };
    
private:
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

class MyCollider
{
public:
    MyCollider() = default;
    virtual ~MyCollider() = default;
    virtual bool checkCollision(const Particle& p) = 0;
};

class PlanCollider : public MyCollider
{
public:
    PlanCollider(const Vec2& point, const Vec2& normal)
        : pointOnPlane(point), planeNormal(normal) {}
    
    bool checkCollision(const Particle& p) override {
        Vec2 p_to_plane;
        p_to_plane.x = p.pos.x - pointOnPlane.x;
        p_to_plane.y = p.pos.y - pointOnPlane.y;
        float distance = p_to_plane.x * planeNormal.x + p_to_plane.y * planeNormal.y;
        return distance < p.radius;
    }

    Point getPointOnPlane() const { return pointOnPlane; }
    Vec2 getPlaneNormal() const { return planeNormal; }

private:
    Point pointOnPlane;
    Vec2 planeNormal;
};

class SphereCollider : public MyCollider
{
public:
    SphereCollider(const Point& center, float radius)
        : sphereCenter(center), sphereRadius(radius) {}
    
    bool checkCollision(const Particle& p) override {
        Vec2 p_to_center;
        p_to_center.x = p.pos.x - sphereCenter.x;
        p_to_center.y = p.pos.y - sphereCenter.y;
        float distanceSq = p_to_center.x * p_to_center.x + p_to_center.y * p_to_center.y;
        float radiusSum = p.radius + sphereRadius;
        return distanceSq < (radiusSum * radiusSum);
    }

    Point getSphereCenter() const { return sphereCenter; }
    float getSphereRadius() const { return sphereRadius; }

private:
    Point sphereCenter;
    float sphereRadius;
};

#endif // CONTEXT_H