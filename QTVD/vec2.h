#ifndef VEC2_H
#define VEC2_H


#include <stdio.h>
#include <math.h>

class Vec2
{
public:
    Vec2() : x(0.0f),y(0.0f) {}
    Vec2(float f1, float f2) : x(f1),y(f2) {}

    void add(Vec2 vec) {
        x += vec.x;
        y += vec.y;
    }
    void add(float f1, float f2) {
        x += f1;
        y += f2;
    }

    float distanceTo(Vec2 otherPoint) {
        return (float) sqrt(pow(x - otherPoint.x, 2) + pow(y - otherPoint.y, 2));
    }

    static float distance(Vec2 originPoint, Vec2 otherPoint) {
        return (float) sqrt(pow(originPoint.x - otherPoint.x, 2) +
                            pow(originPoint.y - otherPoint.y, 2));
    }

    float length() {
        return (float)sqrt(pow(x, 2) + pow(y, 2));
    }

    static Vec2 multi(Vec2 orgVec2, float times) {
//        float nx = orgVec2.x * times;
//        float ny = orgVec2.y * times;
//        return Vec2(nx, ny);
        return Vec2(orgVec2.x * times, orgVec2.y * times);
    }

public:
    float x;
    float y;
};

#endif // VEC2_H
