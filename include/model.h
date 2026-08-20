//
// Created by 布朗尼蛋糕 on 2026/8/12.
//

#pragma once

typedef struct Model
{
    float *vertices;
    int *indices;

}Model;

void InitModelFormFile(char **argv);