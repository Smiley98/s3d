#pragma once
#include <cstdio>

inline void PrintV2(float* v)
{
    printf("x: %f y: %f\n", v[0], v[1]);
}

inline void PrintV3(float* v)
{
    printf("x: %f y: %f z: %f\n", v[0], v[1], v[2]);
}

inline void PrintV4(float* v)
{
    printf("x: %f y: %f z: %f w: %f\n", v[0], v[1], v[2], v[3]);
}