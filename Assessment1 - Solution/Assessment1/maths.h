#pragma once
#ifndef MATHS_H
#define MATHS_H

#include <../glm/glm.hpp>

/*
struct vec3
{
    float x, y, z;
};
*/



struct vertex
{
    glm::vec4 pos;
    glm::vec3 col;
    glm::vec3 nor;
};

struct triangle
{
public:
    vertex v1, v2, v3;
    bool reflect;
    int primID;
};

#endif //MATHS_H
