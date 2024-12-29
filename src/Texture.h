#pragma once
#include <glad/glad.h>

// Should I make this to prevent GL casting aids, or store something like GLenum binding; within Texture struct?
//enum TextureType
//{
//    TEXTURE_1D,
//    TEXTURE_2D,
//    TEXTURE_3D,
//    TEXTURE_CUBEMAP,
//    TEXTURE_TYPE_COUNT
//};

// TODO -- Unify 1d-texture, 2d-texture, 3d-texture, and cubemap into single structure otherwise validation and overall API will be out of control.
struct Texture
{
    GLuint id = GL_NONE;
    int width = -1;
    int height = -1;

    // Storing this might create a disconnect between intended GPU-state vs *actual* GPU-state
    //GLuint slot = 0;

    int internalFormat = -1;
    int format = -1;
    int type = -1;
    int filter = -1;
};

// New API???
//void CreateTexture1D();
//void CreateTexture2D();
//void CreateTexture3D();
//void CreateTextureCubemap();

void CreateTextureFromFile(Texture* texture, const char* path, bool flip = false);
void CreateTextureFromMemory(Texture* texture, int width, int height, int internalFormat, int format, int type, int filter, unsigned char* pixels = nullptr);
void DestroyTexture(Texture* texture);

void BindTexture(Texture texture, GLuint slot);
void UnbindTexture(Texture texture, GLuint slot);

struct Cubemap
{
    // Cubemap doesn't need to be as flexible as Texture (for current use-cases) so data is minimal.
    GLuint id = GL_NONE;
};

void CreateCubemap(Cubemap* cubemap, const char* path[6]);
void DestroyCubemap(Cubemap* cubemap);

void BindCubemap(Cubemap cubemap);
void UnbindCubemap(Cubemap cubemap);

void CreateTextures();
void DestroyTextures();
