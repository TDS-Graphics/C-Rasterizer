//
// Created by 布朗尼蛋糕 on 2026/8/11.
//

#pragma once
#include "types.h"

void ComputeBarycentric(Vertex triangle[3], float x, float y, float *alpha, float *beta, float *gamma);
void Rasterize(Vertex triangle[3], RGB *framebuffer, float *zbuffer, int scene_width, int scene_height);

void pureColorShader(RGB *framebuffer, int scene_width, int scene_height, int idx_y, int idx_x, RGB color);
void noiseShader(RGB *framebuffer, int scene_width, int scene_height, int idx_y, int idx_x);
void lerpVertexColorShader(RGB *framebuffer, int scene_width, int scene_height, int idx_y, int idx_x);
void phongShader();