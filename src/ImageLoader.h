#pragma once
#include <cstdint>

uint8_t* LoadImage1D(const char* file, int* width, int* channels);
uint8_t* LoadImage2D(const char* file, int* width, int* height, int* channels, bool flip = true);
uint8_t* LoadImage3D(const char* file, int* width, int* height, int* depth, int* channels);
void UnloadImage(uint8_t* pixels);

void LoadCubemap(const char* name, const char* extension, int* width, int* height, int* channels, uint8_t* pixels[6]);
void UnloadCubemap(uint8_t* pixels[6]);
