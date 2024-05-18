#pragma once
#include <glad/glad.h>
#include <cassert>

// Consider making a Texture struct to enforce static typing & resemble Image.
struct Image;
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
