//
// Created by 布朗尼蛋糕 on 2026/8/11.
//

#include "helpers.h"
#include <stdlib.h>
#include <math.h>

float clamp(float lower, float upper, float in)
{
    if (in > upper) {return upper;}
    else if (in < lower) {return lower;}
    else {return in;}
}

void Rotate2D(Vec3 *v, float centerX, float centerY, float angle)
{
    float x = v->x, y = v->y;
    v->x = centerX + (x-centerX) * cos(angle) - (y-centerY) * sin(angle);
    v->y = centerY + (x-centerX) * sin(angle) + (y-centerY) * cos(angle);
}