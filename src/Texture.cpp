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

GLuint GenTextureId()
{
    GLuint id;
    glGenTextures(1, &id);
    glBindTexture(GL_TEXTURE_2D, id);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glBindTexture(GL_TEXTURE_2D, GL_NONE);
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

void LoadTexture(Texture* texture, int width, int height)
{
    assert(texture->id == GL_NONE);

    texture->id = GenTextureId();
    texture->width = width;
    texture->height = height;

    BindTexture(*texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    UnbindTexture(*texture);
}

void UnloadTexture(Texture* texture)
{
    GLint current;
    glGetIntegerv(GL_TEXTURE_BINDING_2D, &current);
    assert(texture->id != current, "Can't delete a bound texture");
    assert(texture->id != GL_NONE, "Invalid texture handle");

    glDeleteTextures(1, &texture->id);
    texture->width = 0;
    texture->height = 0;
    texture->id = GL_NONE;
}

void UpdateTexture(Texture texture, const Image& image)
{
    assert(!image.pixels.empty());
    assert(texture.id != GL_NONE);

    BindTexture(texture);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, image.width, image.height, GL_RGBA, GL_UNSIGNED_BYTE, image.pixels.data());
    UnbindTexture(texture);
}
