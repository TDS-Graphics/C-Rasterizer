//
// Created by 布朗尼蛋糕 on 2026/8/11.
//

#include "rasterizer.h"
#include "helpers.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

void pureColorShader(RGB *framebuffer, int scene_width, int scene_height, int idx_y, int idx_x, RGB color)
{
    framebuffer[idx_y * scene_width + idx_x] = color;
}

void noiseShader(RGB *framebuffer, int scene_width, int scene_height, int idx_y, int idx_x)
{
    float randColor_r = (float)rand() / RAND_MAX;
    float randColor_g = (float)rand() / RAND_MAX;
    float randColor_b = (float)rand() / RAND_MAX;
    framebuffer[idx_y * scene_width + idx_x].r = randColor_r;
    framebuffer[idx_y * scene_width + idx_x].g = randColor_g;
    framebuffer[idx_y * scene_width + idx_x].b = randColor_b;
}

void lerpVertexColorShader(RGB *framebuffer, int scene_width, int scene_height, int idx_y, int idx_x)
{

}

void PhongShader()
{

}

void ComputeBarycentric(Vertex triangle[3], float x, float y, float *alpha, float *beta, float *gamma)
{
    const float xp = x, xa = triangle[0].vPosition.x, xb = triangle[1].vPosition.x, xc = triangle[2].vPosition.x;
    const float yp = y, ya = triangle[0].vPosition.y, yb = triangle[1].vPosition.y, yc = triangle[2].vPosition.y;
    const float det = (xa - xc)*(yb - yc) - (xb - xc)*(ya - yc);

    *alpha = ((xp - xc)*(yb - yc) - (xb - xc)*(yp - yc)) / det;
    *beta = ((xa - xc)*(yp - yc) - (xp - xc)*(ya - yc)) / det;
    *gamma = 1 - *alpha - *beta;
}

void Rasterize(Vertex triangle[3], RGB *framebuffer, float *zbuffer, int scene_width, int scene_height)
{
#ifdef DEBUG
    printf("Entry of Rasterizer!\n");
    RGB color;
    color.r = (float)rand() / RAND_MAX;
    color.g = (float)rand() / RAND_MAX;
    color.b = (float)rand() / RAND_MAX;
#endif

    int min_x = (int)fminf(triangle[0].vPosition.x, fminf(triangle[1].vPosition.x, triangle[2].vPosition.x));
    int max_x = (int)fmaxf(triangle[0].vPosition.x, fmaxf(triangle[1].vPosition.x, triangle[2].vPosition.x));
    int min_y = (int)fminf(triangle[0].vPosition.y, fminf(triangle[1].vPosition.y, triangle[2].vPosition.y));
    int max_y = (int)fmaxf(triangle[0].vPosition.y, fmaxf(triangle[1].vPosition.y, triangle[2].vPosition.y));

    // scissor test
    if (min_x < 0) {min_x = 0;}
    if (max_x > scene_width) {max_x = scene_width;}
    if (min_y < 0) {min_y = 0;}
    if (max_y > scene_height) {max_y = scene_height;}

#ifdef DEBUG
    printf("AABB constructed!\n");
#endif

    for (int i = min_y; i < max_y; ++i)
    {
        for(int j = min_x; j < max_x; ++j)
        {
            float y = i + 0.5f, x = j + 0.5f;
            float alpha = 0.f, beta = 0.f, gamma = 0.f;

            ComputeBarycentric(triangle, x, y, &alpha, &beta, &gamma);

            if (alpha >= -EPS && beta >= -EPS && gamma >= -EPS)
            {
            #if defined(DEBUG) && defined(WRITEPPM)
                printf("Tested! Pixel (%d, %d) is in triangle!\n", i, j);
            #endif
                // haven't perpective projection yet. assume orthogonal.
                float z_lerp = 
                    triangle[0].vPosition.z * alpha + 
                    triangle[1].vPosition.z * beta + 
                    triangle[2].vPosition.z * gamma;
                if (z_lerp < zbuffer[i * scene_width + j]) // depth test
                {
                    zbuffer[i * scene_width + j] = z_lerp;
                    // Call pixel shader here.
                    pureColorShader(framebuffer, scene_width, scene_height, i, j, color);
                }
            }
        }
    }
}