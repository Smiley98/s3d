#pragma once
#include <glad/glad.h>
#include <cassert>
#include <vector>
#include "Color.h"

// Must pass pixel by reference otherwise vector copy-constructor is called.
using Pixels = std::vector<Color>;

struct Image
{
    Pixels pixels;
    int width = 0;
    int height = 0;
};

void LoadImage(Image& image, int width, int height);
void UnloadImage(Image& image);
void Fill(Image& image, Color color);
void Flip(Image& image);

GLuint LoadTexture(const char* path, bool flip = false);
GLuint LoadTexture(const Image& image);
void UnloadTexture(GLuint& id);
void UpdateTexture(GLuint id, const Image& image);

inline void BindTexture(GLuint id, GLuint slot = 0)
{
    // TODO -- consider making an object to represent GL state?
    // Might help students understand & even debug!
    assert(id != GL_NONE);
    glActiveTexture(GL_TEXTURE0 + slot);
    glBindTexture(GL_TEXTURE_2D, id);
}

inline void UnbindTexture(GLuint id)
{
    assert(id != GL_NONE);
    glBindTexture(GL_TEXTURE_2D, GL_NONE);
}
