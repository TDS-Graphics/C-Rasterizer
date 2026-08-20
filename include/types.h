//
// Created by 布朗尼蛋糕 on 2026/8/11.
//

#pragma once

typedef struct Vec2 {float x, y;} Vec2;
typedef struct Vec3 {float x, y, z;} Vec3;
typedef struct Vec4 {float x, y, z, w;} Vec4;
typedef struct Mat2 {Vec2 row1, row2;} Mat2;
typedef struct Mat3 {Vec3 row1, row2, row3;} Mat3;
typedef struct Mat4 {Vec4 row1, row2, row3, row4;} Mat4;

typedef struct RGB
{
    float r, g, b;
}RGB;

typedef struct Vertex
{
    Vec3 vPosition;
    RGB vColor;
}Vertex;