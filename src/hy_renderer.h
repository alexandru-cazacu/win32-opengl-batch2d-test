#pragma once

#include <stdint.h>

//~
// Colors
//

/// RGBA float colors.
typedef struct {
    float r;
    float g;
    float b;
    float a;
} HyColor;

// TODO(alex): Move into theme
// TODO(alex): Create own header file.
uint32_t white = 0xFFFFFFFF;
uint32_t bg0 = 0x282828FF;
uint32_t bg1 = 0x3C3836FF;
uint32_t bg0_s = 0x32302FFF;
uint32_t gray = 0x928374FF;
uint32_t red0 = 0xCC241dFF;
uint32_t red1 = 0xFB4939FF;
uint32_t green0 = 0x98971AFF;
uint32_t green1 = 0xB8BB26FF;
uint32_t purple0 = 0xB16286FF;
uint32_t purple1 = 0xD3869BFF;
uint32_t aqua0 = 0x689D6AFF;
uint32_t aqua1 = 0x8EC07CFF;
uint32_t fg = 0xebdbb2ff;

// TODO(alex): Remove?
internal HyColor HyWhite = { 1.0f, 1.0f, 1.0f, 1.0f };
internal HyColor HyDebugBg = { 0.0f, 0.0f, 0.0f, 0.75f };
#if 0
internal float HyColor_Grey2[] = {0.2f, 0.2f, 0.2f, 1.0f};
internal float HyColor_Grey1[] = {0.1f, 0.1f, 0.1f, 1.0f};
internal float HyColor_Grey05[] = {0.05f, 0.05f, 0.05f, 1.0f};
internal float HyColor_Grey[] = {0.1f, 0.1f, 0.1f, 1.0f};
internal float HyColor_Black[] = {0.0f, 0.0f, 0.0f, 1.0f};
internal float HyColor_Red[] = {1.0f, 0.0f, 0.0f, 1.0f};
internal float HyColor_Green[] = {0.0f, 1.0f, 0.0f, 1.0f};
internal float HyColor_Blue[] = {0.0f, 0.0f, 1.0f, 1.0f};
internal float HyColor_Magenta[] = {1.0f, 0.0f, 1.0f, 1.0f};
internal float HyColor_Transparent[] = {0.0f, 0.0f, 0.0f, 0.0f};
#endif

internal HyColor hex_to_HyColor(uint32_t val)
{
    HyColor result = {0};
    result.r = ((val >> 24) & 0xff) / 255.0f;
    result.g = ((val >> 16) & 0xff) / 255.0f;
    result.b = ((val >> 8) & 0xff) / 255.0f;
    result.a = ((val) & 0xff) / 255.0f;
    
    return result;
}

//~
// Vectors
//

typedef struct { float x, y; } V2;
typedef struct { float x, y, z; } V3;
typedef struct { float x, y, z, w; } V4;

//~
// Textures
//

typedef struct HyTexture HyTexture;

typedef enum HyTextureFilterMode { HyTextureFilterMode_Linear, HyTextureFilterMode_Nearest } HyTextureFilterMode;

internal HyTexture* hy_texture_create(const char* path, HyTextureFilterMode filter);
internal void hy_texture_destroy(HyTexture* texture);
internal void hy_texture_bind(HyTexture* texture, uint32_t location);

//~
// Framebuffer
//

typedef struct HyFramebuffer HyFramebuffer;

internal HyFramebuffer HyFramebuffer_Create(uint32_t width, uint32_t height);
internal void          HyFramebuffer_Destroy(HyFramebuffer* framebuffer);
internal void          HyFramebuffer_Resize(HyFramebuffer* framebuffer, uint32_t width, uint32_t height);
internal void          HyFramebuffer_Bind(HyFramebuffer* framebuffer);
internal void          HyFramebuffer_Unbind();

//~
// Shader
//

#define HY_SHADER_MAP_SIZE 16

typedef struct HyShader HyShader;
typedef struct HyShaderLibrary HyShaderLibrary;

internal void HY_Shader_CheckCompileErrors(uint32_t shader, const char* type);
internal HyShader* HY_Shader_Create(const char* vertFilePath, const char* fragFilePath);
internal int HY_Shader_GetUniformLocation(HyShader* shader, const char* name);
internal void HY_Shader_Delete(HyShader* shader);
internal void HY_Shader_Bind(HyShader* shader);
internal void HY_Shader_SetInt(HyShader* shader, const char* name, int value);
internal void HY_Shader_SetFloat4(HyShader* shader, const char* name, const vec4 value);
internal void HY_Shader_SetFloat3(HyShader* shader, const char* name, const vec3* value);
internal void HY_Shader_SetFloat(HyShader* shader, const char* name, float value);
internal void HY_Shader_SetMat4(HyShader* shader, char* name, mat4 value);

//~
// Camera
//

typedef struct HyCamera2D HyCamera2D;

internal void HyCamera2D_Resize(HyCamera2D* camera, float width, float height, float nearPlane, float farPlane);
internal HyCamera2D HyCamera2D_Create(float width, float height, float nearPlane, float farPlane);

//~
// Renderer2D
//

typedef struct HyRenderer2D HyRenderer2D;
typedef struct HyRenderer2DStats HyRenderer2DStats;

internal void hy_renderer2d_init();
internal void hy_renderer2d_shutdown();
internal void hy_renderer2d_begin_scene(HyCamera2D* camera);
internal void hy_renderer2d_end_scene();
internal void hy_renderer2d_flush();

internal void draw_quad_3tcc(vec3 pos, float width, float height, HyTexture* hyTexture, HyColor color, float tx,
                             float ty, float tw, float th);
internal void draw_quad_3tc(vec3 pos, vec2 size, HyTexture* hyTexture, HyColor color);
internal void draw_quad_3c(vec3 pos, vec2 size, HyColor color);
internal void draw_quad_2tc(float x, float y, vec2 size, HyTexture* hyTexture, HyColor color);
internal void draw_quad_2c(vec2 pos, vec2 size, HyColor color);
internal void draw_debug_text(const char* string, float x, float y, HyColor color);

internal void HY_SetClearColorCmd(HyColor* color);
internal void HY_SetClearColorCmdByColors(float r, float g, float b, float a);
internal void HY_ClearCmd();
internal void HY_ClearColorCmd();

internal HyRenderer2DStats hy_renderer2d_get_stats();
internal void hy_renderer2d_reset_stats();
