#pragma once
#include <cmath>
#include <cstdint>
#include <cstdlib>

struct Color
{
    uint8_t r = 0;
    uint8_t g = 0;
    uint8_t b = 0;
    uint8_t a = 0;
};

constexpr Color RED = { 0xFF, 0x00, 0x00, 0xFF };
constexpr Color GREEN = { 0x00, 0xFF, 0x00, 0xFF };
constexpr Color BLUE = { 0x00, 0x00, 0xFF, 0xFF };

constexpr Color CYAN = { 0x00, 0xFF, 0xFF, 0xFF };
constexpr Color MAGENTA = { 0xFF, 0x00, 0xFF, 0xFF };
constexpr Color YELLOW = { 0xFF, 0xFF, 0x00, 0xFF };

constexpr Color WHITE = { 0xFF, 0xFF, 0xFF, 0xFF };
constexpr Color GRAY = { 0x80, 0x80, 0x80, 0xFF };
constexpr Color BLACK = { 0x00, 0x00, 0x00, 0xFF };

inline Color Float2ToColor(float* v)
{
    Color color;
    color.r = fmaxf(0.0f, fminf(v[0], 1.0f)) * 255.0f;
    color.g = fmaxf(0.0f, fminf(v[1], 1.0f)) * 255.0f;
    return color;
}

inline Color Float3ToColor(float* v)
{
    Color color;
    color.r = fmaxf(0.0f, fminf(v[0], 1.0f)) * 255.0f;
    color.g = fmaxf(0.0f, fminf(v[1], 1.0f)) * 255.0f;
    color.b = fmaxf(0.0f, fminf(v[2], 1.0f)) * 255.0f;
    return color;
}

inline Color Float4ToColor(float* v)
{
    Color color;
    color.r = fmaxf(0.0f, fminf(v[0], 1.0f)) * 255.0f;
    color.g = fmaxf(0.0f, fminf(v[1], 1.0f)) * 255.0f;
    color.b = fmaxf(0.0f, fminf(v[2], 1.0f)) * 255.0f;
    color.a = fmaxf(0.0f, fminf(v[3], 1.0f)) * 255.0f;
    return color;
}

inline Color RandRGB()
{
    Color color;
    color.r = rand() % 255;
    color.g = rand() % 255;
    color.b = rand() % 255;
    color.a = 255;
    return color;
}

// Unused (same for converting Color[0, 255] to Float[0, 1]).
//inline Color RandRGBA()
//{
//    Color color;
//    color.r = rand() % 255;
//    color.g = rand() % 255;
//    color.b = rand() % 255;
//    color.a = rand() % 255;
//    return color;
//}
