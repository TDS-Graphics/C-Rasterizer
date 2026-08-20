//
// Created by 布朗尼蛋糕 on 2026/8/10.
//

/*
纯无聊用纯 C 写着玩的，当时怎么舒服怎么写。你会体验到包括但不限于：
混用接口命名规则、随性的宏和变量命名规则、中英文随性混写注释，如果有严重的钻牛角尖癖好建议别看。
btw, i love repeat myself. fuck design patterns.
*/

#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "SDL3/SDL.h"
#include "SDL3/SDL_main.h"
#include "types.h"
#include "helpers.h"
#include "rasterizer.h"

//#define WRITEPPM
#define scene_width 1280
#define scene_height 800
#define NUM_TRIANGLE 4

int main(int argc, char **argv)
{
    /* 
    实现了一个简易的随机着色器（in rasterizer.c），需要在光栅化器的循环体外统一 all 一次 srand(time) 确保随机数不重复。
    * 由于 time 记录的是自 1970 年以来的秒数，而调用一次 Rasterize() 的速度极快（微秒级别），几乎没有时间差异，
    * 在光栅化器内部调用 srand(time) 大概率生成一样的随机数序列，导致四个三角形颜色相同。
    * 所以在主函数里调用一次 srand 就行了
    */
    srand(time(NULL));

    // init SDL3
    if (!SDL_Init(SDL_INIT_VIDEO))
    {
        SDL_Log("SDL INIT FAILED: %s\n", SDL_GetError());
        return 1;
    }

    SDL_Window *window = NULL;
    SDL_Renderer *renderer = NULL;

    if (!SDL_CreateWindowAndRenderer("Rasterizer Window (SDL3)", scene_width, scene_height, 0, &window, &renderer))
    {
        SDL_Log("CREATE WINDOW / RENDERER FAILED: %s\n", SDL_GetError());
        SDL_Quit();
        return 2;
    }

    SDL_Texture *texture = SDL_CreateTexture(
        renderer,
        SDL_PIXELFORMAT_RGB24,
        SDL_TEXTUREACCESS_STREAMING,
        scene_width,
        scene_height
    );

    if (!texture) {
        SDL_Log("CREATE TEXTURE FAILED: %s\n", SDL_GetError());
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 3;
    }

    // renderer backend buffers (framebuffer and zbuffer)
    RGB *framebuffer = (RGB *)calloc(scene_width * scene_height, sizeof(RGB));
    if (framebuffer == NULL) {printf("FRAMEBUFFER CREATE FAILED!\n"); return 1919;}
    float *zbuffer = (float *)calloc(scene_width * scene_height, sizeof(float));
    if (zbuffer == NULL) {printf("Z-BUFFER CREATE FAILED!\n"); return 810;}

    Vertex triangle1[3] =
    {
        {{630, 130, 0}, {1.0f, 0.0f, 0.0f}},   // v1
        {{580, 580, 0}, {0.0f, 1.0f, 0.0f}},   // v2
        {{680, 320, 0}, {0.0f, 0.0f, 1.0f}}   // v3
    };

    Vertex triangle2[3] =
    {
        {{250, 160, 0}, {1.0f, 1.0f, 0.0f}},
        {{750, 80, 0}, {1.0f, 0.0f, 1.0f}},
        {{520, 50, 0}, {0.0f, 1.0f, 1.0f}}
    };

    Vertex triangle3[3] =
    {
        {{220, 520, 0}, {1.0f, 0.5f, 0.0f}},
        {{410, 260, 0}, {0.0f, 0.5f, 1.0f}},
        {{450, 560, 0}, {0.5f, 1.0f, 0.0f}}
    };

    Vertex triangle4[3] =
    {
        {{500, 300, 0}, {1.0f, 0.0f, 0.5f}},
        {{770, 240, 0}, {0.0f, 1.0f, 0.5f}},
        {{680, 520, 0}, {0.5f, 0.0f, 1.0f}}
    };

    Rasterize(triangle1, framebuffer, zbuffer, scene_width, scene_height);
    Rasterize(triangle2, framebuffer, zbuffer, scene_width, scene_height);
    Rasterize(triangle3, framebuffer, zbuffer, scene_width, scene_height);
    Rasterize(triangle4, framebuffer, zbuffer, scene_width, scene_height);

#ifdef WRITEPPM
    FILE* fp = fopen("binary.ppm", "wb");
    if (fp == NULL) {printf("FILE CREATE FAILED!\n"); return 114514;}
    (void)fprintf(fp, "P6\n%d %d\n255\n", scene_width, scene_height);
    for (int i = 0; i < scene_height * scene_width; ++i)
    {
        static unsigned char color[3];
        // Tonemapping or gamma correction
        color[0] = (unsigned char)(255 * pow(clamp(0., 1., (float)framebuffer[i].r), 0.6f));
        color[1] = (unsigned char)(255 * pow(clamp(0., 1., (float)framebuffer[i].g), 0.6f));
        color[2] = (unsigned char)(255 * pow(clamp(0., 1., (float)framebuffer[i].b), 0.6f));
        fwrite(color, 1, 3, fp);
    }
    fclose(fp);
#endif

    unsigned char *pixel_buffer = (unsigned char *)malloc(scene_width * scene_height * 3);
    if (pixel_buffer == NULL)
    {
        printf("SDL3 PIXELBUFFER CREATE FAILED!\n");
        free(framebuffer);
        return 364364;
    }

    // SDL3 Main Loop
    bool running = true;
    while (running)
    {
        // init and set rotate parameters
        float angle = SDL_GetTicks() / 1000.f * 1.5f;
        float centerX = 0.f, centerY = 0.f;

        // init buffers per frame
        memset(framebuffer, 0, scene_width * scene_height * sizeof(RGB));
        memset(zbuffer, 1, scene_width * scene_height * sizeof(float));

        // init and render triangle1 in real-time
        Vertex tri1[3];
        memcpy(tri1, triangle1, sizeof(triangle1));
        centerX = (tri1[0].vPosition.x + tri1[1].vPosition.x + tri1[2].vPosition.x) / 3;
        centerY = (tri1[0].vPosition.y + tri1[1].vPosition.y + tri1[2].vPosition.y) / 3;
        for (int j = 0; j < 3; ++j)
        {
            Rotate2D(&tri1[j].vPosition, centerX, centerY, angle);
        }
        Rasterize(tri1, framebuffer, zbuffer, scene_width, scene_height);

        // init and render triangle2 in real-time
        Vertex tri2[3];
        memcpy(tri2, triangle2, sizeof(triangle2));
        centerX = (tri2[0].vPosition.x + tri2[1].vPosition.x + tri2[2].vPosition.x) / 3;
        centerY = (tri2[0].vPosition.y + tri2[1].vPosition.y + tri2[2].vPosition.y) / 3;
        for (int j = 0; j < 3; ++j)
        {
            Rotate2D(&tri2[j].vPosition, centerX, centerY, angle);
        }
        Rasterize(tri2, framebuffer, zbuffer, scene_width, scene_height);

        // init and render triangle3 in real-time
        Vertex tri3[3];
        memcpy(tri3, triangle3, sizeof(triangle3));
        centerX = (tri3[0].vPosition.x + tri3[1].vPosition.x + tri3[2].vPosition.x) / 3;
        centerY = (tri3[0].vPosition.y + tri3[1].vPosition.y + tri3[2].vPosition.y) / 3;
        for (int j = 0; j < 3; ++j)
        {
            Rotate2D(&tri3[j].vPosition, centerX, centerY, angle);
        }
        Rasterize(tri3, framebuffer, zbuffer, scene_width, scene_height);

        // init and render triangle4 in real-time
        Vertex tri4[3];
        memcpy(tri4, triangle4, sizeof(triangle4));
        centerX = (tri4[0].vPosition.x + tri4[1].vPosition.x + tri4[2].vPosition.x) / 3;
        centerY = (tri4[0].vPosition.y + tri4[1].vPosition.y + tri4[2].vPosition.y) / 3;
        for (int j = 0; j < 3; ++j)
        {
            Rotate2D(&tri4[j].vPosition, centerX, centerY, angle);
        }
        Rasterize(tri4, framebuffer, zbuffer, scene_width, scene_height);
        
        
        // write SDL3 pixel buffer
        for (int i = 0; i < scene_height * scene_width; ++i)
        {
            // Tonemapping or to say gamma correction
            pixel_buffer[i * 3 + 0] = (unsigned char)(255 * pow(clamp(0., 1., (float)framebuffer[i].r), 0.6f));
            pixel_buffer[i * 3 + 1] = (unsigned char)(255 * pow(clamp(0., 1., (float)framebuffer[i].g), 0.6f));
            pixel_buffer[i * 3 + 2] = (unsigned char)(255 * pow(clamp(0., 1., (float)framebuffer[i].b), 0.6f));
        }

        // update texture to gpu
        SDL_UpdateTexture(texture, NULL, pixel_buffer, scene_width * 3);

        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_EVENT_QUIT)
            {
                running = false;
            }
        }
        // clear screen
        SDL_RenderClear(renderer);
        // draw texture on screen
        SDL_RenderTexture(renderer, texture, NULL, NULL);
        // present frame
        SDL_RenderPresent(renderer);
    }


    free(pixel_buffer);
    free(framebuffer);

    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}