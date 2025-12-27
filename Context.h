#ifndef CONTEXT_H
#define CONTEXT_H

#include <vector>
#include <iostream>

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

class MyCollider;

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
        : pointOnPlane(point)
    {
        float length = std::sqrt(normal.x * normal.x + normal.y * normal.y);
        planeNormal.x = normal.x / length;
        planeNormal.y = normal.y / length;
    }
    
    bool checkCollision(const Particle& p) override {
        Vec2 p_to_plane;
        p_to_plane.x = p.predicted_pos.x - pointOnPlane.x;
        p_to_plane.y = p.predicted_pos.y - pointOnPlane.y;
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

class BarCollider : public MyCollider
{
public:
    BarCollider(const Point& start, const Point& end, float thickness)
        : startPoint(start), endPoint(end), barThickness(thickness) {}
    
    bool checkCollision(const Particle& p) override {
        Vec2 ab;
        ab.x = endPoint.x - startPoint.x;
        ab.y = endPoint.y - startPoint.y;

        Vec2 ap;
        ap.x = p.predicted_pos.x - startPoint.x;
        ap.y = p.predicted_pos.y - startPoint.y;

        float ab_squared = ab.x * ab.x + ab.y * ab.y;
        float ap_ab = ap.x * ab.x + ap.y * ab.y;
        float t = ap_ab / ab_squared;

        t = std::fmax(0.0f, std::fmin(1.0f, t));

        Vec2 closest;
        closest.x = startPoint.x + t * ab.x;
        closest.y = startPoint.y + t * ab.y;

        Vec2 pc;
        pc.x = p.predicted_pos.x - closest.x;
        pc.y = p.predicted_pos.y - closest.y;

        float distanceSq = pc.x * pc.x + pc.y * pc.y;
        float radiusSum = p.radius + barThickness / 2.0f;

        return distanceSq < (radiusSum * radiusSum);
    }

    Point getStartPoint() const { return startPoint; }
    Point getEndPoint() const { return endPoint; }
    float getBarThickness() const { return barThickness; }

private:
    Point startPoint;
    Point endPoint;
    float barThickness;
};

#endif // CONTEXT_H