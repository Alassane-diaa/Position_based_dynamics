#ifndef COLLIDER_H
#define COLLIDER_H

#include <cmath>

// Forward declarations pour éviter les dépendances circulaires
struct Vec2;
struct Particle;

using Point = Vec2;

struct Vec2 {
    float x;
    float y;
};

struct Particle {
    Vec2 pos;
    Vec2 predicted_pos;
    Vec2 velocity;
    float radius;
    float mass;
};

class MyCollider
{
public:
    MyCollider() = default;
    virtual ~MyCollider() = default;
    virtual bool checkCollision(const Particle& p) = 0;
    virtual void solveCollision(Particle& p) = 0;
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
    
    bool checkCollision(const Particle& p) override;
    void solveCollision(Particle& p) override;

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
    
    bool checkCollision(const Particle& p) override;
    void solveCollision(Particle& p) override;

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
    
    bool checkCollision(const Particle& p) override;
    void solveCollision(Particle& p) override;

    Point getStartPoint() const { return startPoint; }
    Point getEndPoint() const { return endPoint; }
    float getBarThickness() const { return barThickness; }

private:
    Point startPoint;
    Point endPoint;
    float barThickness;
};

class PipeCollider : public MyCollider
{
public:
    PipeCollider(float x, float gapCenterY, float gapHeight = 120.0f) 
        : posX(x), gapY(gapCenterY), gapSize(gapHeight) {}
    virtual ~PipeCollider() = default;

    bool checkCollision(const Particle& p) override;
    void solveCollision(Particle& p) override;
    
    float& getPosX() { return posX; }
    float getGapY() const { return gapY; }
    float getGapSize() const { return gapSize; }

    static constexpr float width = 50.0f; 

private:
    float posX;
    float gapY;
    float gapSize;   
};


#endif // COLLIDER_H