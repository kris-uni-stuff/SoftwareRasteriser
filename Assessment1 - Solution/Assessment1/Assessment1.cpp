#include <iostream>
#include <SDL.h>
#undef main

#include "display.h"
#include "solution.h"
#include "bmp.h"

#include <../glm/gtc/matrix_transform.hpp>


SDL_Event event;
SDL_Renderer* renderer;
SDL_Window* window;

bool close = false;

/*

void COMP3011_CreateWindow(int w, int h)
{
    SDL_Init(SDL_INIT_VIDEO);
    SDL_CreateWindowAndRenderer(w, h, 0, &window, &renderer);
}

bool COMP3011_WindowShouldClose()
{
    return close;
}


void COMP3011_SwapBuffers()
{
    for (int pixel_y = 0; pixel_y < PIXEL_H; pixel_y++)
    {
        for (int pixel_x = 0; pixel_x < PIXEL_W; pixel_x++)
        {
            float pixel_r = display_buffer[pixel_x][PIXEL_H - pixel_y].x;
            float pixel_g = display_buffer[pixel_x][PIXEL_H - pixel_y].y;
            float pixel_b = display_buffer[pixel_x][PIXEL_H - pixel_y].z;
            SDL_SetRenderDrawColor(renderer, pixel_r, pixel_g, pixel_b, 255);
            SDL_RenderDrawPoint(renderer, pixel_x, pixel_y);
        }
    }
    SDL_RenderPresent(renderer);
}

void COMP3011_PollEvents()
{
    if (SDL_PollEvent(&event) && event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE)
    {
        close = true;
    }
}

void COMP3011_Deinit()
{
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}
*/

void CounterEndAndPrint(LARGE_INTEGER StartingTime, LARGE_INTEGER* EndingTime, LARGE_INTEGER Frequency)
{
    QueryPerformanceCounter(EndingTime);

    LARGE_INTEGER ElapsedMicroseconds;
    ElapsedMicroseconds.QuadPart = (*EndingTime).QuadPart - StartingTime.QuadPart;
    ElapsedMicroseconds.QuadPart *= 1000000;
    ElapsedMicroseconds.QuadPart /= Frequency.QuadPart;
    std::cout << (float)ElapsedMicroseconds.QuadPart / (float)1000000 << std::endl;
}


int main()
{
    LARGE_INTEGER Frequency;
    QueryPerformanceFrequency(&Frequency);

    //SetNumSamples(4);

    //clear the screen
    float bgd[] = { 1.f, 1.f, 1.f, 1.f };
    ClearColourBuffer(bgd);

    vector<Object> objs;
    std::vector<triangle> tris;

    const std::string MODEL_PATH = "objs/cornell2/cornell-box.obj";
//    const std::string MODEL_PATH = "objs/bird/textured_quad.obj";

    obj_parse(MODEL_PATH.c_str(), &objs, 1.f);


    AppendTriangles(&tris, objs);


    glm::mat4 M(1);
    M = glm::translate(M, vec3(0.1, -2.5, -6));

    glm::mat4 V(1);

    glm::mat4 P(1);
    P = glm::perspective(glm::radians(60.f), (float)PIXEL_W/(float)PIXEL_H, 0.1f, 10.f);

    for (int t = 0; t < tris.size(); t++)
    {
        Translate(M, V, P, &tris[t]);
    }


    for (int t = 0; t < tris.size(); t++)
    {
        TransformToScreenSpace(PIXEL_W, PIXEL_H, &tris[t]);
    }




    LARGE_INTEGER Render_StartingTime, Render_EndingTime;
    QueryPerformanceCounter(&Render_StartingTime);

    Rasterise(tris);

    CounterEndAndPrint(Render_StartingTime, &Render_EndingTime, Frequency);



    savebitmap("render.bmp", display_buffer, PIXEL_W, PIXEL_H);

}
