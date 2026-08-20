//
// Created by 布朗尼蛋糕 on 2026/8/11.
//

#pragma once
#include "types.h"
#define EPS 1e-9

float clamp(float lower, float upper, float in);
void Rotate2D(Vec3 *v, float cx, float cy, float angle);