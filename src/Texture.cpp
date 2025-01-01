#include "Texture.h"
#include <unordered_map>
#include <cassert>

GLint GetTexture1DId();
GLint GetTexture2DId();
GLint GetTexture3DId();
GLint GetCubemapId();

static std::unordered_map<GLuint, GLuint> fTex1Units;
static std::unordered_map<GLuint, GLuint> fTex2Units;
static std::unordered_map<GLuint, GLuint> fTex3Units;
static std::unordered_map<GLuint, GLuint> fTexCubemapUnits;

void CreateTexture1D(Texture1D* texture, int width, int internalFormat, int format, int type, int filter, void* pixels)
{
    GLuint id = GL_NONE;
    GLenum target = GL_TEXTURE_1D;
    glGenTextures(1, &id);
    glBindTexture(target, id);
    glTexParameteri(target, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(target, GL_TEXTURE_MIN_FILTER, filter);
    glTexParameteri(target, GL_TEXTURE_MAG_FILTER, filter);
    glTexImage1D(target, 0, internalFormat, width, 0, format, type, pixels);
    glBindTexture(target, GL_NONE);
    texture->id = id;
}

void CreateTexture2D(Texture2D* texture, int width, int height, int internalFormat, int format, int type, int filter, void* pixels)
{
    GLuint id = GL_NONE;
    GLenum target = GL_TEXTURE_2D;
    glGenTextures(1, &id);
    glBindTexture(target, id);
    glTexParameteri(target, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(target, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(target, GL_TEXTURE_MIN_FILTER, filter);
    glTexParameteri(target, GL_TEXTURE_MAG_FILTER, filter);
    glTexImage2D(target, 0, internalFormat, width, height, 0, format, type, pixels);
    glBindTexture(target, GL_NONE);
    texture->id = id;
}

void CreateTexture3D(Texture3D* texture, int width, int height, int depth, int internalFormat, int format, int type, int filter, void* pixels)
{
    GLuint id = GL_NONE;
    GLenum target = GL_TEXTURE_3D;
    glGenTextures(1, &id);
    glBindTexture(target, id);
    glTexParameteri(target, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(target, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(target, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(target, GL_TEXTURE_MIN_FILTER, filter);
    glTexParameteri(target, GL_TEXTURE_MAG_FILTER, filter);
    glTexImage3D(target, 0, internalFormat, width, height, depth, 0, format, type, pixels);
    glBindTexture(target, GL_NONE);
    texture->id = id;
}

void CreateCubemap(Cubemap* texture, int width, int height, int internalFormat, int format, int type, int filter, void* pixels[6])
{
    GLuint id = GL_NONE;
    GLenum target = GL_TEXTURE_CUBE_MAP;
    glGenTextures(1, &id);
    glBindTexture(target, id);
    glTexParameteri(target, GL_TEXTURE_MIN_FILTER, filter);
    glTexParameteri(target, GL_TEXTURE_MAG_FILTER, filter);
    glTexParameteri(target, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(target, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(target, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    for (int i = 0; i < 6; i++)
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, internalFormat, width, height, 0, format, type, pixels[i]);
    glBindTexture(target, GL_NONE);
    texture->id = id;
}

void DestroyTexture1D(Texture1D* texture)
{
    glDeleteTextures(1, &texture->id);
    texture->id = GL_NONE;
}

void DestroyTexture2D(Texture2D* texture)
{
    glDeleteTextures(1, &texture->id);
    texture->id = GL_NONE;
}

void DestroyTexture3D(Texture3D* texture)
{
    glDeleteTextures(1, &texture->id);
    texture->id = GL_NONE;
}

void DestroyCubemap(Cubemap* texture)
{
    glDeleteTextures(1, &texture->id);
    texture->id = GL_NONE;
}

void BindTexture1D(Texture1D texture, GLuint unit)
{
    glActiveTexture(GL_TEXTURE0 + unit);
    assert(GetTexture1DId() == GL_NONE);
    assert(fTex1Units.insert({ texture.id, unit }).second);
    glBindTexture(GL_TEXTURE_1D, texture.id);
}

void BindTexture2D(Texture2D texture, GLuint unit)
{
    glActiveTexture(GL_TEXTURE0 + unit);
    assert(GetTexture2DId() == GL_NONE);
    assert(fTex2Units.insert({ texture.id, unit }).second);
    glBindTexture(GL_TEXTURE_2D, texture.id);
}

void BindTexture3D(Texture3D texture, GLuint unit)
{
    glActiveTexture(GL_TEXTURE0 + unit);
    assert(GetTexture3DId() == GL_NONE);
    assert(fTex3Units.insert({ texture.id, unit }).second);
    glBindTexture(GL_TEXTURE_3D, texture.id);
}

void BindCubemap(Cubemap texture, GLuint unit)
{
    glActiveTexture(GL_TEXTURE0 + unit);
    assert(GetCubemapId() == GL_NONE);
    assert(fTexCubemapUnits.insert({ texture.id, unit }).second);
    glBindTexture(GL_TEXTURE_CUBE_MAP, texture.id);
}

void UnbindTexture1D(Texture1D texture, GLuint unit)
{
    glActiveTexture(GL_TEXTURE0 + unit);
    glBindTexture(GL_TEXTURE_1D, GL_NONE);
    assert(fTex1Units.at(texture.id) == unit);
    assert(fTex1Units.erase(texture.id) == 1);
}

void UnbindTexture2D(Texture2D texture, GLuint unit)
{
    glActiveTexture(GL_TEXTURE0 + unit);
    glBindTexture(GL_TEXTURE_2D, GL_NONE);
    assert(fTex2Units.at(texture.id) == unit);
    assert(fTex2Units.erase(texture.id) == 1);
}

void UnbindTexture3D(Texture3D texture, GLuint unit)
{
    glActiveTexture(GL_TEXTURE0 + unit);
    glBindTexture(GL_TEXTURE_3D, GL_NONE);
    assert(fTex3Units.at(texture.id) == unit);
    assert(fTex3Units.erase(texture.id) == 1);
}

void UnbindCubemap(Cubemap texture, GLuint unit)
{
    glActiveTexture(GL_TEXTURE0 + unit);
    glBindTexture(GL_TEXTURE_CUBE_MAP, GL_NONE);
    assert(fTexCubemapUnits.at(texture.id) == unit);
    assert(fTexCubemapUnits.erase(texture.id) == 1);
}

GLint GetTexture1DId()
{
    GLint id;
    glGetIntegerv(GL_TEXTURE_BINDING_1D, &id);
    return id;
}

GLint GetTexture2DId()
{
    GLint id;
    glGetIntegerv(GL_TEXTURE_BINDING_2D, &id);
    return id;
}

GLint GetTexture3DId()
{
    GLint id;
    glGetIntegerv(GL_TEXTURE_BINDING_3D, &id);
    return id;
}

GLint GetCubemapId()
{
    GLint id;
    glGetIntegerv(GL_TEXTURE_BINDING_CUBE_MAP, &id);
    return id;
}
