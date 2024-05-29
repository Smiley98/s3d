#pragma once
#include <glad/glad.h>
#include <cassert>

struct Image;
struct Texture
{
    GLuint id = GL_NONE;
    int width = 0;
    int height = 0;
};

GLuint LoadTexture(const char* path, bool flip = false);
GLuint LoadTexture(const Image& image);
void UnloadTexture(GLuint& id);
void UpdateTexture(GLuint id, const Image& image);

inline void BindTexture(GLuint id, GLuint slot = 0)
{
    assert(id != GL_NONE);
    glActiveTexture(GL_TEXTURE0 + slot);
    glBindTexture(GL_TEXTURE_2D, id);
}

inline void UnbindTexture(GLuint id)
{
    assert(id != GL_NONE);
    glBindTexture(GL_TEXTURE_2D, GL_NONE);
}

void LoadTexture(Texture* texture, int width, int height);
void UnloadTexture(Texture* texture);

void UpdateTexture(Texture texture, const Image& image);

inline void BindTexture(Texture texture, GLuint slot = 0)
{
    assert(texture.id != GL_NONE);
    glActiveTexture(GL_TEXTURE0 + slot);
    glBindTexture(GL_TEXTURE_2D, texture.id);
}

inline void UnbindTexture(Texture texture)
{
    assert(texture.id != GL_NONE);
    glBindTexture(GL_TEXTURE_2D, GL_NONE);
}