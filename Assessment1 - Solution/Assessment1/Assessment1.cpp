#include <iostream>
#include <SDL.h>
#undef main

#include "display.h"
#include "solution.h"
#include "bmp.h"
#include "utility.h"





int main()
{
    LARGE_INTEGER Frequency;
    QueryPerformanceFrequency(&Frequency);

    LARGE_INTEGER Render_StartingTime, Render_EndingTime;
    QueryPerformanceCounter(&Render_StartingTime);

    const std::string MODEL_PATH = "objs/cornell2/cornell-box.obj";
    //const std::string MODEL_PATH = "objs/bird/textured_quad.obj";
    render(MODEL_PATH);

    savebitmap("render.bmp", colour_buffer, PIXEL_W, PIXEL_H);

    CounterEndAndPrint(Render_StartingTime, &Render_EndingTime, Frequency);
}
