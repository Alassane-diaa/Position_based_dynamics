#ifndef CONTEXTFB_H
#define CONTEXTFB_H

#include "Context.h"

class ContextFB : public Context
{
public:
    ContextFB() : Context() {}
    ~ContextFB() = default;
    void updatePhysicalSystem(float dt);
    
private:
    void addStaticContactConstraints(float dt);
    void movePipes(float dt);
};

class PipeCollider : public MyCollider
{
public:
    PipeCollider(float x, float gapCenterY, float gapHeight = 120.0f) 
        : posX(x), gapY(gapCenterY), gapSize(gapHeight) {}
    virtual ~PipeCollider() = default;

    bool checkCollision(const Particle& p) override {
        float half_width = width / 2.0f;
        
        if (p.predicted_pos.x + p.radius < posX - half_width ||
            p.predicted_pos.x - p.radius > posX + half_width) {
            return false; 
        }
        
        float gapTop = gapY + gapSize / 2.0f;
        float gapBottom = gapY - gapSize / 2.0f;
        if (p.predicted_pos.y - p.radius > gapBottom && 
            p.predicted_pos.y + p.radius < gapTop) {
            return false; 
        }
        
        return true;
    }
    
    float& getPosX() { return posX; }
    float getGapY() const { return gapY; }
    float getGapSize() const { return gapSize; }

    static constexpr float width = 50.0f; 

private:
    float posX;
    float gapY;
    float gapSize;   
};


#endif // CONTEXTFB_H