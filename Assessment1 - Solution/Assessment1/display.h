#pragma once
#ifndef DISPLAY_H
#define DISPLAY_H

#include "maths.h"

#define PIXEL_W 1920
#define PIXEL_H 1080

float display_buffer[PIXEL_W * PIXEL_H * 3];
float depth_buffer[PIXEL_W * PIXEL_H];



#endif //DISPLAY_H
