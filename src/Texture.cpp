#include "Texture.h"
#include "Image.h"

GLuint GenTexture()
{
    GLuint id;
    glGenTextures(1, &id);
    BindTexture(id);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    return id;
}

GLuint LoadTexture(const char* path, bool flip)
{
    // Support 3 & 4 channels for GPU textures in the future.
    // For now its best to force 4 channels since the focus is CPU rendering. 
    //GLenum format = image.channels == 3 ? GL_RGB : GL_RGBA;
    GLenum format = GL_RGBA;

    Image image;
    LoadImage(image, path, flip);

    GLuint id = GenTexture();
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image.width, image.height,
        0, format, GL_UNSIGNED_BYTE, image.pixels.data());
    
    UnbindTexture(id);
    return id;
}

GLuint LoadTexture(const Image& image)
{
    assert(!image.pixels.empty());

    GLuint id = GenTexture();
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image.width, image.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image.pixels.data());
    
    UnbindTexture(id);
    return id;
}

void UnloadTexture(GLuint& id)
{
    GLint current;
    glGetIntegerv(GL_TEXTURE_BINDING_2D, &current);

    assert(id != current, "Can't delete a bound texture");
    glDeleteTextures(1, &id);
}

void UpdateTexture(GLuint id, const Image& image)
{
    assert(!image.pixels.empty());
    BindTexture(id);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, image.width, image.height, GL_RGBA, GL_UNSIGNED_BYTE, image.pixels.data());
    UnbindTexture(id);
}
