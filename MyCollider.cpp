#include "MyCollider.h"
#include <algorithm>
#include <cmath>

bool PlanCollider::checkCollision(const Particle& p) {
    Vec2 p_to_plane;
    p_to_plane.x = p.predicted_pos.x - pointOnPlane.x;
    p_to_plane.y = p.predicted_pos.y - pointOnPlane.y;
    float distance = p_to_plane.x * planeNormal.x + p_to_plane.y * planeNormal.y;
    return distance < p.radius;
}

void PlanCollider::solveCollision(Particle& p) {
    Vec2 p_to_plane;
    p_to_plane.x = p.predicted_pos.x - pointOnPlane.x;
    p_to_plane.y = p.predicted_pos.y - pointOnPlane.y;

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

bool SphereCollider::checkCollision(const Particle& p) {
    Vec2 p_to_center;
    p_to_center.x = p.pos.x - sphereCenter.x;
    p_to_center.y = p.pos.y - sphereCenter.y;
    float distanceSq = p_to_center.x * p_to_center.x + p_to_center.y * p_to_center.y;
    float radiusSum = p.radius + sphereRadius;
    return distanceSq < (radiusSum * radiusSum);
}

void SphereCollider::solveCollision(Particle& p) {
    Vec2 p_to_center;
    p_to_center.x = p.predicted_pos.x - sphereCenter.x;
    p_to_center.y = p.predicted_pos.y - sphereCenter.y;
    float distance = sqrt(p_to_center.x * p_to_center.x + p_to_center.y * p_to_center.y);
    float minDistance = p.radius + sphereRadius;

    if (distance < 0.0001f) { 
        p_to_center = Vec2{0.0f, 10.0f}; 
        distance = 0.0001f;
    }

    Vec2 n;
    n.x = p_to_center.x / distance;
    n.y = p_to_center.y / distance;

    p.predicted_pos.x = sphereCenter.x + n.x * minDistance;
    p.predicted_pos.y = sphereCenter.y + n.y * minDistance;
}

bool BarCollider::checkCollision(const Particle& p) {
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

void BarCollider::solveCollision(Particle& p) {
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
    float distance = sqrt(pc.x * pc.x + pc.y * pc.y);
    float radiusSum = p.radius + barThickness / 2.0f;
    if (distance < 0.0001f) {
        pc = Vec2{0.0f, 10.0f};
        distance = 0.0001f;
    }
    Vec2 n;
    n.x = pc.x / distance;
    n.y = pc.y / distance;
    float C = distance - radiusSum;
    p.predicted_pos.x -= n.x * C;
    p.predicted_pos.y -= n.y * C;
}

bool PipeCollider::checkCollision(const Particle& p) {
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

void PipeCollider::solveCollision(Particle& p) {
    float half_width = width / 2.0f;
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