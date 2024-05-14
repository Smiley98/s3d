#pragma once
#include <cstdint>

struct Color
{
    uint8_t r = 0;
    uint8_t g = 0;
    uint8_t b = 0;
    uint8_t a = 0;
};

struct Colorf
{
    float r = 0.0f;
    float g = 0.0f;
    float b = 0.0f;
    float a = 0.0f;
};

inline Color Convert(Colorf colorf)
{
    Color color;
    color.r = colorf.r * 255.0f;
    color.g = colorf.g * 255.0f;
    color.b = colorf.b * 255.0f;
    color.a = colorf.a * 255.0f;
    return color;
}

inline Colorf Convert(Color color)
{
    Colorf colorf;
    colorf.r = color.r / 255.0f;
    colorf.g = color.g / 255.0f;
    colorf.b = color.b / 255.0f;
    colorf.a = color.a / 255.0f;
    return colorf;
}

constexpr Color RED = { 0xFF, 0x00, 0x00, 0xFF };
constexpr Color GREEN = { 0x00, 0xFF, 0x00, 0xFF };
constexpr Color BLUE = { 0x00, 0x00, 0xFF, 0xFF };

constexpr Color CYAN = { 0x00, 0xFF, 0xFF, 0xFF };
constexpr Color MAGENTA = { 0xFF, 0x00, 0xFF, 0xFF };
constexpr Color YELLOW = { 0xFF, 0xFF, 0x00, 0xFF };

constexpr Color WHITE = { 0xFF, 0xFF, 0xFF, 0xFF };
constexpr Color GRAY = { 0x80, 0x80, 0x80, 0xFF };
constexpr Color BLACK = { 0x00, 0x00, 0x00, 0xFF };