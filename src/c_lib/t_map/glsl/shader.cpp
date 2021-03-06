/* Gnomescroll, Copyright (c) 2013 Symbolic Analytics
 * Licensed under GPLv3 */
#include "shader.hpp"

#include <t_map/glsl/settings.hpp>
#include <t_map/glsl/texture.hpp>

#include <SDL/texture_sheet_loader.hpp>

#include <SDL/hald_clut.hpp>

/*
In GL 3.0, GL_GENERATE_MIPMAP is deprecated, and in 3.1 and above, it was removed.
So for those versions, you must use glGenerateMipmap.

GL_GENERATE_MIPMAP is supported for 1.4 and over
*/

//glGenerateMipmap(GL_TEXTURE_2D);  //Generate mipmaps now!!!
//glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE);

namespace t_map
{

class MapShader map_shader;
class MapCompatibilityShader map_compatibility_shader;

SDL_Surface *terrain_map_surface = NULL;
GLuint terrain_map_texture = 0;

GLuint block_textures_compatibility = 0; //use for intel mode

void init_shaders()
{
    //determine support for anisotropic filtering
    //if (true || !GLEW_texture_array)

    printf("Generate clut texture... ");
    generate_clut_texture();
    printf("finished\n");

    if (GLEW_EXT_texture_array)
    {
        printf("GLEW_EXT_texture_array supposedly supported\n");
    }
    else
    {
        printf("Warning: GLEW_EXT_texture_array not supported. Using Backup Shader.\n");
        T_MAP_BACKUP_SHADER = 1;
    }

    if (!GLEW_EXT_texture_sRGB)
    {
        printf("Warning: GLEW_EXT_texture_sRGB not supported.\n");
    }


/*
    if (!GLEW_EXT_multitexture)
    {
        printf("!!! Warning: GL_ARB_multitexture not supported. \n")
    }
*/
    //ANISOTROPIC_FILTERING = 0;

    if (ANISOTROPIC_FILTERING)
    {
        if (GLEW_EXT_texture_filter_anisotropic) // ANISOTROPY_EXT
        {
            glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &ANISOTROPY_LARGEST_SUPPORTED);
            printf("Anisotropic filtering supported: Max supported is %f\n", ANISOTROPY_LARGEST_SUPPORTED);
        }
        else
        {
            printf("Anisotropic filtering is not supported\n");
            ANISOTROPIC_FILTERING = 0;
        }
    }

    T_MAP_BACKUP_SHADER = 0;

    if (T_MAP_BACKUP_SHADER == 0)
    {
        //set_map_shader_0();
        //init_map_3d_texture();

        map_shader.init_texture();
        map_shader.init_shader();
        //true on error
        //if (shader_error_occured(map_shader.shader->shader))

        if (!map_shader.shader->shader_valid)
        {
            printf("!!! Default map shader failed. Setting backup shader \n");
            T_MAP_BACKUP_SHADER = 1;
        }
    }

    if (T_MAP_BACKUP_SHADER == 1)
    {
        ANISOTROPIC_FILTERING = 0;  //disable anisotropic filtering

        printf("!!! Warning: Using Intel GPU Compatability mode shader level 0\n");

        map_compatibility_shader.init_texture();
        map_compatibility_shader.init_shader(0);

        if (!map_compatibility_shader.shader->shader_valid)
        {
            printf("!!! shader level 0 failed.  Using backup shader level 1 \n");
            map_compatibility_shader.init_shader(1);
        }
    }

    init_block_texture_normal();

    //GS_ASSERT(map_shader == NULL);
    //GS_ASSERT(map_compatibility_shader == NULL);
    //map_shader = new MapShader;
    //map_compatibility_shader = new MapCompatibilityShader;
}

void reload_textures()
{
    if (T_MAP_BACKUP_SHADER)
        map_compatibility_shader.init_texture();
    else
        map_shader.init_texture();
}

void MapShader::init_texture()
{
    //printf("init_map_3d_texture: 1 \n");
    //glEnable(GL_TEXTURE_2D);

    glDisable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, 0);

    if (terrain_map_glsl != 0)
        glDeleteTextures(1, &terrain_map_glsl);
    glGenTextures(1, &terrain_map_glsl);

    glBindTexture(GL_TEXTURE_2D_ARRAY, terrain_map_glsl);

    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    //GL_MIRRORED_REPEAT
    //glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP);
    //glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP);

    //glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_REPEAT);
    //glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_REPEAT);

    //glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, ANISOTROPY_LARGEST_SUPPORTED);

    if (ANISOTROPIC_FILTERING)
    {
        glTexParameterf(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAX_ANISOTROPY_EXT, ANISOTROPY_LARGEST_SUPPORTED);
    }

    if (T_MAP_TEXTURE_2D_ARRAY_MIPMAPS == 0)
    {
        glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        //glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, T_MAP_MAG_FILTER ? GL_NEAREST : GL_LINEAR);

        switch (T_MAP_MAG_FILTER)
        {
            case 0:
                glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST); break;
            case 1:
                glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR); break;
            default:
                printf("Error: T_MAP_MAG_FILTER value %i invalid for non-mipmapped GL_TEXTURE_2D_ARRAY \n", T_MAP_MAG_FILTER);
                glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR); break;
        }

    }
    else
    {
        //GL_LINEAR_MIPMAP_LINEAR, GL_NEAREST_MIPMAP_LINEAR, GL_LINEAR_MIPMAP_NEAREST, and GL_NEAREST_MIPMAP_NEAREST
        glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        //glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST_MIPMAP_NEAREST);
        //glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST_MIPMAP_LINEAR);
        //glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, T_MAP_MAG_FILTER ? GL_LINEAR_MIPMAP_NEAREST : GL_LINEAR_MIPMAP_LINEAR);
        //glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);

        switch (T_MAP_MAG_FILTER)
        {
            case 0:
                glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR); break;
            case 1:
                glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR); break;
            case 2:
                glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST); break;
            case 3:
                glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST); break;
            default:
                printf("Error: T_MAP_MAG_FILTER value %i invalid for mipmapped GL_TEXTURE_2D_ARRAY \n", T_MAP_MAG_FILTER);
                glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR); break;
        }

        glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_BASE_LEVEL, 0);
        glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAX_LEVEL, 8);

        glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_GENERATE_MIPMAP, GL_TRUE);
    }

    GS_ASSERT(TextureSheetLoader::cube_texture_sheet_loader->surface != NULL);
    if (TextureSheetLoader::cube_texture_sheet_loader->surface == NULL) return;

    GLenum format = get_texture_format(TextureSheetLoader::cube_texture_sheet_loader->surface);
    GLuint internalFormat = GL_SRGB8_ALPHA8_EXT; //GL_RGBA;
    //GLuint internalFormat = GL_SRGB8_ALPHA8; //GL_RGBA;

    const int w = TextureSheetLoader::cube_texture_sheet_loader->tile_size;    //32 or 16
    const int h = TextureSheetLoader::cube_texture_sheet_loader->tile_size;    //32 or 16
    const int d = 256;

    GS_ASSERT(TextureSheetLoader::cube_texture_sheet_loader->texture_stack != NULL);
    if (TextureSheetLoader::cube_texture_sheet_loader->texture_stack == NULL) return;
    glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, internalFormat, w, h, d, 0, format, GL_UNSIGNED_BYTE, TextureSheetLoader::cube_texture_sheet_loader->texture_stack);

    glBindTexture(GL_TEXTURE_2D_ARRAY, 0);

    glDisable(GL_TEXTURE_2D);
}

void MapCompatibilityShader::init_texture()
{
    SDL_Surface* s = TextureSheetLoader::cube_texture_sheet_loader->surface;

    GS_ASSERT(s != NULL);
    if (s == NULL) return;

    glEnable(GL_TEXTURE_2D);

    if (block_textures_compatibility != 0)
        glDeleteTextures(1, &block_textures_compatibility);
    glGenTextures(1, &block_textures_compatibility);

    glBindTexture(GL_TEXTURE_2D, block_textures_compatibility);

    // Set the texture's stretching properties

    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    GLuint internalFormat = GL_SRGB8_ALPHA8_EXT; //GL_RGBA;

    // Edit the texture object's image data using the information SDL_Surface gives us
    //glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, terrain_map_surface->w, terrain_map_surface->h, 0, GL_RGBA, GL_UNSIGNED_BYTE, terrain_map_surface->pixels); //2nd parameter is level

    if (ANISOTROPIC_FILTERING)
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, ANISOTROPY_LARGEST_SUPPORTED);

    GLenum texture_format = get_texture_format(s);
    glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, s->w, s->h, 0, texture_format, GL_UNSIGNED_BYTE, s->pixels); //2nd parameter is level
    glDisable(GL_TEXTURE_2D);
}

void toggle_3d_texture_settings()
{
    static int s = 0;
    s = (s+1) % 6;

    switch (s)
    {
        case 0:
            T_MAP_TEXTURE_2D_ARRAY_MIPMAPS = 0;
            T_MAP_MAG_FILTER  = 0;
            break;
        case 1:
            T_MAP_TEXTURE_2D_ARRAY_MIPMAPS = 0;
            T_MAP_MAG_FILTER  = 1;
            break;
        case 2:
            T_MAP_TEXTURE_2D_ARRAY_MIPMAPS = 1;
            T_MAP_MAG_FILTER  = 0;
            break;
        case 3:
            T_MAP_TEXTURE_2D_ARRAY_MIPMAPS = 1;
            T_MAP_MAG_FILTER  = 1;
            break;
        case 4:
            T_MAP_TEXTURE_2D_ARRAY_MIPMAPS = 1;
            T_MAP_MAG_FILTER  = 2;
            break;
        case 5:
            T_MAP_TEXTURE_2D_ARRAY_MIPMAPS = 1;
            T_MAP_MAG_FILTER  = 3;
            break;
        default:
            printf("toggle_3d_texture_settings: error \n");
            T_MAP_TEXTURE_2D_ARRAY_MIPMAPS = 1;
            T_MAP_MAG_FILTER  = 0;
    }
    printf("TEXTURE_SETTING: T_MAP_TEXTURE_2D_ARRAY_MIPMAPS = %i T_MAP_MAG_FILTER = %i \n", T_MAP_TEXTURE_2D_ARRAY_MIPMAPS,T_MAP_MAG_FILTER);
}

void init_block_texture_normal()
{
    SDL_Surface* s = TextureSheetLoader::cube_texture_sheet_loader->surface;
    IF_ASSERT(s == NULL) return;

    glEnable(GL_TEXTURE_2D);
    if (block_textures_normal == 0)
        glGenTextures(1, &block_textures_normal);

    glBindTexture(GL_TEXTURE_2D, block_textures_normal);
    // Set the texture's stretching properties
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    GLenum texture_format = get_texture_format(s);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, s->w, s->h, 0, texture_format, GL_UNSIGNED_BYTE, s->pixels); //2nd parameter is level
    glDisable(GL_TEXTURE_2D);
}

void teardown_shader()
{
}

}   // t_map
