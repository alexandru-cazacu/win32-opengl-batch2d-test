// This is an independent project of an individual developer. Dear PVS-Studio,
// please check it.

// PVS-Studio Static Code Analyzer for C, C++, C#, and Java:
// http://www.viva64.com

// TODO(alex): Indexed font rendering like 4coder.
// TODO(alex): Font rendering with stb_truetype.
// TODO(alex): Font caching (safe img to disk to avoid computing at startup).

#define MAINICON 101

//#define _WIN32_WINNT 0x0601 // Targets Windows 7 or later
//#include <sdkddkver.h>

// Disables unused Windows functions and makes build times faster.
// Commented out the functions we actually need.

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#ifndef WIN32_EXTRA_LEAN
#define WIN32_EXTRA_LEAN // TODO(alex): Check if it really does something in Windows.h
#endif

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN // Only for MFC, but who knows.
#endif

#if 0
#define NOGDICAPMASKS
//#define NOSYSMETRICS
#define NOMENUS
#define NOICONS
#define NOSYSCOMMANDS
#define NORASTEROPS
#define OEMRESOURCE
#define NOATOM
#define NOCLIPBOARD
//#define NOCOLOR
//#define NOCTLMGR
#define NODRAWTEXT
#define NOKERNEL
#define NONLS
#define NOMEMMGR
#define NOMETAFILE
#define NOOPENFILE
#define NOSCROLL
#define NOSERVICE
#define NOSOUND
//#define NOTEXTMETRIC
#define NOWH
#define NOCOMM
#define NOKANJI
#define NOHELP
#define NOPROFILER
#define NODEFERWINDOWPOS
#define NOMCX
#define NORPC
#define NOPROXYSTUB
#define NOIMAGE
#define NOTAPE
#define NOMINMAX
#define STRICT
#endif

#include <windows.h>
#include <windowsx.h>
#include <shellapi.h>
#include <uxtheme.h>
#include <dwmapi.h>
#include <versionhelpers.h>

#define HY_EDITOR_CAPTION_W 46
#define HY_EDITOR_CAPTION_H 30
#define HY_EDITOR_STATUS_H 20

#include "hy_types.c"
#include "hy_assert.c"
#include "hy_log.c"
#include "hy_platform.h"

#pragma warning(push)
#pragma warning(disable : 4204) // nonstandard extension used : non-constant aggregate initializer
#pragma warning(disable : 4996) // TODO(alex): What error
#pragma warning(disable : 4459) // TODO(alex): What error
#pragma warning(disable : 4244) // TODO(alex): What error
#pragma warning(disable : 4267) // TODO(alex): What error
#pragma warning(disable : 4456) // TODO(alex): What error

#include <cglm/cglm.h>
#include <cglm/cam.h>
#include <ini.c>

#define FGL_IMPLEMENTATION
#include <final_dynamic_opengl.h>

// STB Image
#define STB_IMAGE_IMPLEMENTATION
#define STBI_MALLOC(size)           hy_malloc(size)
#define STBI_REALLOC(ptr,newsz)     hy_realloc(ptr, newsz)
#define STBI_FREE(ptr)              hy_free(ptr)
#include <stb_image.h>

// STB Image resize
#define STB_IMAGE_RESIZE_IMPLEMENTATION
#define STBIR_MALLOC(size,context) hy_malloc(size)
#define STBIR_FREE(ptr,context)    hy_free(ptr)
#include <stb_image_resize.h>

// STB Truetype
#define STB_TRUETYPE_IMPLEMENTATION
#define STBTT_MALLOC(size,context) hy_malloc(size)
#define STBTT_FREE(ptr,context)    hy_free(ptr)
#include <stb_truetype.h>

#pragma warning(pop)

#include "hy_file.c"
#include "hy_platform_win32.c"
#include "hy_renderer_win32_ogl.c"
#include "hy_ui.c"

// TODO(alex): What is the right way to add an icon without Visual Studio?

/// Engine startup configuration read from .hyperrc
typedef struct {
    HyWindowStartMode startMode;
    const char*       user;
} HyConfig;

global_variable HyCamera2D camera2D;

internal int configHandler(void* user, const char* section, const char* name, const char* value)
{
    HyConfig* config = (HyConfig*)user;
    
#define MATCH(s, n) strcmp(section, s) == 0 && strcmp(name, n) == 0
    if (MATCH("window", "startMode")) {
        // config->startMode = strdup(value);
        if (strcmp(value, "auto") == 0) {
            config->startMode = HyWindowStartMode_Auto;
        } else if (strcmp(value, "maximized") == 0) {
            config->startMode = HyWindowStartMode_Maximized;
        } else if (strcmp(value, "fullscreen") == 0) {
            config->startMode = HyWindowStartMode_Fullscreen;
        }
    } else if (MATCH("misc", "user")) {
        config->user = _strdup(value);
    } else {
        return 0; /* unknown section/name, error */
    }
    return 1;
}

internal void SizeCallback(HyWindow* hyWindow, unsigned int width, unsigned int height)
{
    // TODO(alex): Remove when use framebuffer
    HyCamera2D_Resize(&camera2D, (float)width, (float)height, -1.0f, 1.0f);
    GL_CALL(glViewport(0, 0, width, height));
}

// Subsystem:console
int main(int argc, char* argv[])
{
    hy_log_init();
    hy_timer_init();
    
    HyConfig config = {0};
    config.startMode = HyWindowStartMode_Auto;
    
    HyFile* configFile = HY_ReadFile(".hypedrc");
    if (configFile) {
        ini_parse_string(configFile->data, configHandler, &config);
        HY_INFO("Config loaded from '.hypedrc'");
        HY_INFO("  - startMode=%d", config.startMode);
        HY_INFO("  - user=%s", config.user);
    } else {
        HY_ERROR(".hypedrc not found");
    }
    
    HyFile* testFile = HY_ReadFile("src/win32_hyper.c");
    
    HyWindow window = {0};
    hy_window_create_borderless(&window, config.startMode, "Hyped");
    hy_set_window_size_callback(&window, SizeCallback);
    
    if (!&window) { // ?
        MessageBox(NULL, "Failed to create window.", "Hyper", MB_ICONERROR);
        ExitProcess(0);
    }
    
    camera2D = HyCamera2D_Create((float)window.width, (float)window.height, -1, 1);
    
    hy_renderer2d_init();
    
    HyTexture* testTexture = hy_texture_create("assets/textures/container.png", HyTextureFilterMode_Linear);
    HyTexture* testTexture1 = hy_texture_create("assets/textures/container_specular.png", HyTextureFilterMode_Linear);
    //HyTexture* asciiTexture = hy_texture_create("assets/textures/DejaVu Sans Mono.png", HyTextureFilterMode_Linear);
    HyTexture* asciiTexture = hy_texture_create("assets/textures/test-msdf.png", HyTextureFilterMode_Linear);
    HyTexture* folderTexture = hy_texture_create("assets/icons/git.png", HyTextureFilterMode_Linear);
    
    // TODO(alex): Move into renderer init struct
    g_renderer.asciiTexture = asciiTexture;
    
    float lastTime = 0.0f;
    
    GL_CALL(glEnable(GL_CULL_FACE));
    GL_CALL(glEnable(GL_BLEND));
    //GL_CALL(glEnable(GL_DEPTH_TEST));
    GL_CALL(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));
    
    HyGliph* glyphs = malloc(sizeof(HyGliph) * 300);
    
    glyphs[32].unicode = 32;
    glyphs[32].advance = 0.27783203125f;
    
    glyphs[33].unicode = 33;
    glyphs[33].advance = 0.27783203125f;
    glyphs[33].plane.left = 0.015380859375f;
    glyphs[33].plane.bottom = -0.07958984375f;
    glyphs[33].plane.right = 0.265380859375f;
    glyphs[33].plane.top = 0.79541015625f;
    glyphs[33].atlas.left = 0.5f;
    glyphs[33].atlas.bottom = 90.5f;
    glyphs[33].atlas.right = 4.5f;
    glyphs[33].atlas.top = 104.5f;
    
    glyphs[34].unicode = 34;
    glyphs[34].advance = 0.35498046875f;
    glyphs[34].plane.left = -0.041748046875f;
    glyphs[34].plane.bottom = 0.370361328125f;
    glyphs[34].plane.right = 0.395751953125f;
    glyphs[34].plane.top = 0.807861328125f;
    glyphs[34].atlas.left = 116.5f;
    glyphs[34].atlas.bottom = 116.5f;
    glyphs[34].atlas.right = 123.5f;
    glyphs[34].atlas.top = 123.5f;
    
    glyphs[35].unicode = 35;
    glyphs[35].advance = 0.55615234375f;
    glyphs[35].plane.left = -0.06689453125f;
    glyphs[35].plane.bottom = -0.07958984375f;
    glyphs[35].plane.right = 0.62060546875f;
    glyphs[35].plane.top = 0.79541015625f;
    glyphs[35].atlas.left = 16.5f;
    glyphs[35].atlas.bottom = 90.5f;
    glyphs[35].atlas.right = 27.5f;
    glyphs[35].atlas.top = 104.5f;
    
    glyphs[36].unicode = 36;
    glyphs[36].advance = 0.55615234375f;
    glyphs[36].plane.left = -0.0400390625f;
    glyphs[36].plane.bottom = -0.19189453125f;
    glyphs[36].plane.right = 0.5849609375f;
    glyphs[36].plane.top = 0.87060546875f;
    glyphs[36].atlas.left = 53.5f;
    glyphs[36].atlas.bottom = 106.5f;
    glyphs[36].atlas.right = 63.5f;
    glyphs[36].atlas.top = 123.5f;
    
    glyphs[37].unicode = 37;
    glyphs[37].advance = 0.88916015625f;
    glyphs[37].plane.left = -0.02587890625f;
    glyphs[37].plane.bottom = -0.117919921875f;
    glyphs[37].plane.right = 0.91162109375f;
    glyphs[37].plane.top = 0.819580078125f;
    glyphs[37].atlas.left = 85.5f;
    glyphs[37].atlas.bottom = 108.5f;
    glyphs[37].atlas.right = 100.5f;
    glyphs[37].atlas.top = 123.5f;
    
    glyphs[38].unicode = 38;
    glyphs[38].advance = 0.6669921875f;
    glyphs[38].plane.left = -0.031494140625f;
    glyphs[38].plane.bottom = -0.081787109375f;
    glyphs[38].plane.right = 0.718505859375f;
    glyphs[38].plane.top = 0.793212890625f;
    glyphs[38].atlas.left = 36.5f;
    glyphs[38].atlas.bottom = 90.5f;
    glyphs[38].atlas.right = 48.5f;
    glyphs[38].atlas.top = 104.5f;
    
    glyphs[39].unicode = 39;
    glyphs[39].advance = 0.19091796875f;
    glyphs[39].plane.left = -0.031005859375f;
    glyphs[39].plane.bottom = 0.370361328125f;
    glyphs[39].plane.right = 0.218994140625f;
    glyphs[39].plane.top = 0.807861328125f;
    glyphs[39].atlas.left = 116.5f;
    glyphs[39].atlas.bottom = 108.5f;
    glyphs[39].atlas.right = 120.5f;
    glyphs[39].atlas.top = 115.5f;
    
    glyphs[40].unicode = 40;
    glyphs[40].advance = 0.3330078125f;
    glyphs[40].plane.left = -0.0087890625f;
    glyphs[40].plane.bottom = -0.3037109375f;
    glyphs[40].plane.right = 0.3662109375f;
    glyphs[40].plane.top = 0.8212890625f;
    glyphs[40].atlas.left = 0.5f;
    glyphs[40].atlas.bottom = 105.5f;
    glyphs[40].atlas.right = 6.5f;
    glyphs[40].atlas.top = 123.5f;
    
    glyphs[41].unicode = 41;
    glyphs[41].advance = 0.3330078125f;
    glyphs[41].plane.left = -0.0087890625f;
    glyphs[41].plane.bottom = -0.3037109375f;
    glyphs[41].plane.right = 0.3662109375f;
    glyphs[41].plane.top = 0.8212890625f;
    glyphs[41].atlas.left = 7.5f;
    glyphs[41].atlas.bottom = 105.5f;
    glyphs[41].atlas.right = 13.5f;
    glyphs[41].atlas.top = 123.5f;
    
    glyphs[42].unicode = 42;
    glyphs[42].advance = 0.38916015625f;
    glyphs[42].plane.left = -0.057373046875f;
    glyphs[42].plane.bottom = 0.35693359375f;
    glyphs[42].plane.right = 0.442626953125f;
    glyphs[42].plane.top = 0.79443359375f;
    glyphs[42].atlas.left = 55.5f;
    glyphs[42].atlas.bottom = 22.5f;
    glyphs[42].atlas.right = 63.5f;
    glyphs[42].atlas.top = 29.5f;
    
    glyphs[43].unicode = 43;
    glyphs[43].advance = 0.583984375f;
    glyphs[43].plane.left = -0.0205078125f;
    glyphs[43].plane.bottom = 0.039794921875f;
    glyphs[43].plane.right = 0.6044921875f;
    glyphs[43].plane.top = 0.664794921875f;
    glyphs[43].atlas.left = 33.5f;
    glyphs[43].atlas.bottom = 19.5f;
    glyphs[43].atlas.right = 43.5f;
    glyphs[43].atlas.top = 29.5f;
    
    glyphs[44].unicode = 44;
    glyphs[44].advance = 0.27783203125f;
    glyphs[44].plane.left = 0.010986328125f;
    glyphs[44].plane.bottom = -0.208251953125f;
    glyphs[44].plane.right = 0.260986328125f;
    glyphs[44].plane.top = 0.166748046875f;
    glyphs[44].atlas.left = 119.5f;
    glyphs[44].atlas.bottom = 53.5f;
    glyphs[44].atlas.right = 123.5f;
    glyphs[44].atlas.top = 59.5f;
    
    glyphs[45].unicode = 45;
    glyphs[45].advance = 0.3330078125f;
    glyphs[45].plane.left = -0.052001953125f;
    glyphs[45].plane.bottom = 0.134033203125f;
    glyphs[45].plane.right = 0.385498046875f;
    glyphs[45].plane.top = 0.384033203125f;
    glyphs[45].atlas.left = 92.5f;
    glyphs[45].atlas.bottom = 25.5f;
    glyphs[45].atlas.right = 99.5f;
    glyphs[45].atlas.top = 29.5f;
    
    glyphs[46].unicode = 46;
    glyphs[46].advance = 0.27783203125f;
    glyphs[46].plane.left = 0.015869140625f;
    glyphs[46].plane.bottom = -0.074951171875f;
    glyphs[46].plane.right = 0.265869140625f;
    glyphs[46].plane.top = 0.175048828125f;
    glyphs[46].atlas.left = 119.5f;
    glyphs[46].atlas.bottom = 48.5f;
    glyphs[46].atlas.right = 123.5f;
    glyphs[46].atlas.top = 52.5f;
    
    glyphs[47].unicode = 47;
    glyphs[47].advance = 0.27783203125f;
    glyphs[47].plane.left = -0.079833984375f;
    glyphs[47].plane.bottom = -0.07958984375f;
    glyphs[47].plane.right = 0.357666015625f;
    glyphs[47].plane.top = 0.79541015625f;
    glyphs[47].atlas.left = 11.5f;
    glyphs[47].atlas.bottom = 75.5f;
    glyphs[47].atlas.right = 18.5f;
    glyphs[47].atlas.top = 89.5f;
    
    glyphs[48].unicode = 48;
    glyphs[48].advance = 0.55615234375f;
    glyphs[48].plane.left = -0.03759765625f;
    glyphs[48].plane.bottom = -0.084228515625f;
    glyphs[48].plane.right = 0.58740234375f;
    glyphs[48].plane.top = 0.790771484375f;
    glyphs[48].atlas.left = 30.5f;
    glyphs[48].atlas.bottom = 75.5f;
    glyphs[48].atlas.right = 40.5f;
    glyphs[48].atlas.top = 89.5f;
    
    glyphs[49].unicode = 49;
    glyphs[49].advance = 0.55615234375f;
    glyphs[49].plane.left = 0.02197265625f;
    glyphs[49].plane.bottom = -0.078125f;
    glyphs[49].plane.right = 0.45947265625f;
    glyphs[49].plane.top = 0.796875f;
    glyphs[49].atlas.left = 41.5f;
    glyphs[49].atlas.bottom = 75.5f;
    glyphs[49].atlas.right = 48.5f;
    glyphs[49].atlas.top = 89.5f;
    
    glyphs[50].unicode = 50;
    glyphs[50].advance = 0.55615234375f;
    glyphs[50].plane.left = -0.045693359374999978f;
    glyphs[50].plane.bottom = -0.078125f;
    glyphs[50].plane.right = 0.57930664062500004f;
    glyphs[50].plane.top = 0.796875f;
    glyphs[50].atlas.left = 49.5f;
    glyphs[50].atlas.bottom = 75.5f;
    glyphs[50].atlas.right = 59.5f;
    glyphs[50].atlas.top = 89.5f;
    
    glyphs[51].unicode = 51;
    glyphs[51].advance = 0.55615234375f;
    glyphs[51].plane.left = -0.0361328125f;
    glyphs[51].plane.bottom = -0.08447265625f;
    glyphs[51].plane.right = 0.5888671875f;
    glyphs[51].plane.top = 0.79052734375f;
    glyphs[51].atlas.left = 60.5f;
    glyphs[51].atlas.bottom = 75.5f;
    glyphs[51].atlas.right = 70.5f;
    glyphs[51].atlas.top = 89.5f;
    
    glyphs[52].unicode = 52;
    glyphs[52].advance = 0.55615234375f;
    glyphs[52].plane.left = -0.05224609375f;
    glyphs[52].plane.bottom = -0.07958984375f;
    glyphs[52].plane.right = 0.57275390625f;
    glyphs[52].plane.top = 0.79541015625f;
    glyphs[52].atlas.left = 71.5f;
    glyphs[52].atlas.bottom = 75.5f;
    glyphs[52].atlas.right = 81.5f;
    glyphs[52].atlas.top = 89.5f;
    
    glyphs[53].unicode = 53;
    glyphs[53].advance = 0.55615234375f;
    glyphs[53].plane.left = -0.03369140625f;
    glyphs[53].plane.bottom = -0.090576171875f;
    glyphs[53].plane.right = 0.59130859375f;
    glyphs[53].plane.top = 0.784423828125f;
    glyphs[53].atlas.left = 90.5f;
    glyphs[53].atlas.bottom = 75.5f;
    glyphs[53].atlas.right = 100.5f;
    glyphs[53].atlas.top = 89.5f;
    
    glyphs[54].unicode = 54;
    glyphs[54].advance = 0.55615234375f;
    glyphs[54].plane.left = -0.03857421875f;
    glyphs[54].plane.bottom = -0.084228515625f;
    glyphs[54].plane.right = 0.58642578125f;
    glyphs[54].plane.top = 0.790771484375f;
    glyphs[54].atlas.left = 99.5f;
    glyphs[54].atlas.bottom = 60.5f;
    glyphs[54].atlas.right = 109.5f;
    glyphs[54].atlas.top = 74.5f;
    
    glyphs[55].unicode = 55;
    glyphs[55].advance = 0.55615234375f;
    glyphs[55].plane.left = -0.033447265625f;
    glyphs[55].plane.bottom = -0.084228515625f;
    glyphs[55].plane.right = 0.591552734375f;
    glyphs[55].plane.top = 0.790771484375f;
    glyphs[55].atlas.left = 87.5f;
    glyphs[55].atlas.bottom = 30.5f;
    glyphs[55].atlas.right = 97.5f;
    glyphs[55].atlas.top = 44.5f;
    
    glyphs[56].unicode = 56;
    glyphs[56].advance = 0.55615234375f;
    glyphs[56].plane.left = -0.0361328125f;
    glyphs[56].plane.bottom = -0.084228515625f;
    glyphs[56].plane.right = 0.5888671875f;
    glyphs[56].plane.top = 0.790771484375f;
    glyphs[56].atlas.left = 98.5f;
    glyphs[56].atlas.bottom = 30.5f;
    glyphs[56].atlas.right = 108.5f;
    glyphs[56].atlas.top = 44.5f;
    
    glyphs[57].unicode = 57;
    glyphs[57].advance = 0.55615234375f;
    glyphs[57].plane.left = -0.03564453125f;
    glyphs[57].plane.bottom = -0.084228515625f;
    glyphs[57].plane.right = 0.58935546875f;
    glyphs[57].plane.top = 0.790771484375f;
    glyphs[57].atlas.left = 0.5f;
    glyphs[57].atlas.bottom = 15.5f;
    glyphs[57].atlas.right = 10.5f;
    glyphs[57].atlas.top = 29.5f;
    
    glyphs[58].unicode = 58;
    glyphs[58].advance = 0.27783203125f;
    glyphs[58].plane.left = 0.015380859375f;
    glyphs[58].plane.bottom = -0.08447265625f;
    glyphs[58].plane.right = 0.265380859375f;
    glyphs[58].plane.top = 0.60302734375f;
    glyphs[58].atlas.left = 34.5f;
    glyphs[58].atlas.bottom = 3.5f;
    glyphs[58].atlas.right = 38.5f;
    glyphs[58].atlas.top = 14.5f;
    
    glyphs[59].unicode = 59;
    glyphs[59].advance = 0.27783203125f;
    glyphs[59].plane.left = 0.010986328125f;
    glyphs[59].plane.bottom = -0.2177734375f;
    glyphs[59].plane.right = 0.260986328125f;
    glyphs[59].plane.top = 0.5947265625f;
    glyphs[59].atlas.left = 0.5f;
    glyphs[59].atlas.bottom = 1.5f;
    glyphs[59].atlas.right = 4.5f;
    glyphs[59].atlas.top = 14.5f;
    
    glyphs[60].unicode = 60;
    glyphs[60].advance = 0.583984375f;
    glyphs[60].plane.left = -0.020751953125f;
    glyphs[60].plane.bottom = 0.040283203125f;
    glyphs[60].plane.right = 0.604248046875f;
    glyphs[60].plane.top = 0.665283203125f;
    glyphs[60].atlas.left = 22.5f;
    glyphs[60].atlas.bottom = 19.5f;
    glyphs[60].atlas.right = 32.5f;
    glyphs[60].atlas.top = 29.5f;
    
    glyphs[61].unicode = 61;
    glyphs[61].advance = 0.583984375f;
    glyphs[61].plane.left = -0.0205078125f;
    glyphs[61].plane.bottom = 0.134521484375f;
    glyphs[61].plane.right = 0.6044921875f;
    glyphs[61].plane.top = 0.572021484375f;
    glyphs[61].atlas.left = 64.5f;
    glyphs[61].atlas.bottom = 22.5f;
    glyphs[61].atlas.right = 74.5f;
    glyphs[61].atlas.top = 29.5f;
    
    glyphs[62].unicode = 62;
    glyphs[62].advance = 0.583984375f;
    glyphs[62].plane.left = -0.020751953125f;
    glyphs[62].plane.bottom = 0.040283203125f;
    glyphs[62].plane.right = 0.604248046875f;
    glyphs[62].plane.top = 0.665283203125f;
    glyphs[62].atlas.left = 44.5f;
    glyphs[62].atlas.bottom = 19.5f;
    glyphs[62].atlas.right = 54.5f;
    glyphs[62].atlas.top = 29.5f;
    
    glyphs[63].unicode = 63;
    glyphs[63].advance = 0.55615234375f;
    glyphs[63].plane.left = -0.03759765625f;
    glyphs[63].plane.bottom = -0.073486328125f;
    glyphs[63].plane.right = 0.58740234375f;
    glyphs[63].plane.top = 0.801513671875f;
    glyphs[63].atlas.left = 50.5f;
    glyphs[63].atlas.bottom = 30.5f;
    glyphs[63].atlas.right = 60.5f;
    glyphs[63].atlas.top = 44.5f;
    
    glyphs[64].unicode = 64;
    glyphs[64].advance = 1.01513671875f;
    glyphs[64].plane.left = -0.0146484375f;
    glyphs[64].plane.bottom = -0.30322265625f;
    glyphs[64].plane.right = 1.0478515625f;
    glyphs[64].plane.top = 0.82177734375f;
    glyphs[64].atlas.left = 35.5f;
    glyphs[64].atlas.bottom = 105.5f;
    glyphs[64].atlas.right = 52.5f;
    glyphs[64].atlas.top = 123.5f;
    
    glyphs[65].unicode = 65;
    glyphs[65].advance = 0.6669921875f;
    glyphs[65].plane.left = -0.07275390625f;
    glyphs[65].plane.bottom = -0.07958984375f;
    glyphs[65].plane.right = 0.73974609375f;
    glyphs[65].plane.top = 0.79541015625f;
    glyphs[65].atlas.left = 110.5f;
    glyphs[65].atlas.bottom = 60.5f;
    glyphs[65].atlas.right = 123.5f;
    glyphs[65].atlas.top = 74.5f;
    
    glyphs[66].unicode = 66;
    glyphs[66].advance = 0.6669921875f;
    glyphs[66].plane.left = -0.000244140625f;
    glyphs[66].plane.bottom = -0.07958984375f;
    glyphs[66].plane.right = 0.687255859375f;
    glyphs[66].plane.top = 0.79541015625f;
    glyphs[66].atlas.left = 13.5f;
    glyphs[66].atlas.bottom = 30.5f;
    glyphs[66].atlas.right = 24.5f;
    glyphs[66].atlas.top = 44.5f;
    
    glyphs[67].unicode = 67;
    glyphs[67].advance = 0.72216796875f;
    glyphs[67].plane.left = -0.0400390625f;
    glyphs[67].plane.bottom = -0.07958984375f;
    glyphs[67].plane.right = 0.7724609375f;
    glyphs[67].plane.top = 0.79541015625f;
    glyphs[67].atlas.left = 61.5f;
    glyphs[67].atlas.bottom = 30.5f;
    glyphs[67].atlas.right = 74.5f;
    glyphs[67].atlas.top = 44.5f;
    
    glyphs[68].unicode = 68;
    glyphs[68].advance = 0.72216796875f;
    glyphs[68].plane.left = -0.001953125f;
    glyphs[68].plane.bottom = -0.07958984375f;
    glyphs[68].plane.right = 0.748046875f;
    glyphs[68].plane.top = 0.79541015625f;
    glyphs[68].atlas.left = 0.5f;
    glyphs[68].atlas.bottom = 30.5f;
    glyphs[68].atlas.right = 12.5f;
    glyphs[68].atlas.top = 44.5f;
    
    glyphs[69].unicode = 69;
    glyphs[69].advance = 0.6669921875f;
    glyphs[69].plane.left = 0.00244140625f;
    glyphs[69].plane.bottom = -0.07958984375f;
    glyphs[69].plane.right = 0.68994140625f;
    glyphs[69].plane.top = 0.79541015625f;
    glyphs[69].atlas.left = 75.5f;
    glyphs[69].atlas.bottom = 30.5f;
    glyphs[69].atlas.right = 86.5f;
    glyphs[69].atlas.top = 44.5f;
    
    glyphs[70].unicode = 70;
    glyphs[70].advance = 0.61083984375f;
    glyphs[70].plane.left = 0.010986328125f;
    glyphs[70].plane.bottom = -0.07958984375f;
    glyphs[70].plane.right = 0.635986328125f;
    glyphs[70].plane.top = 0.79541015625f;
    glyphs[70].atlas.left = 108.5f;
    glyphs[70].atlas.bottom = 45.5f;
    glyphs[70].atlas.right = 118.5f;
    glyphs[70].atlas.top = 59.5f;
    
    glyphs[71].unicode = 71;
    glyphs[71].advance = 0.77783203125f;
    glyphs[71].plane.left = -0.02197265625f;
    glyphs[71].plane.bottom = -0.07958984375f;
    glyphs[71].plane.right = 0.79052734375f;
    glyphs[71].plane.top = 0.79541015625f;
    glyphs[71].atlas.left = 94.5f;
    glyphs[71].atlas.bottom = 45.5f;
    glyphs[71].atlas.right = 107.5f;
    glyphs[71].atlas.top = 59.5f;
    
    glyphs[72].unicode = 72;
    glyphs[72].advance = 0.72216796875f;
    glyphs[72].plane.left = 0.01708984375f;
    glyphs[72].plane.bottom = -0.07958984375f;
    glyphs[72].plane.right = 0.70458984375f;
    glyphs[72].plane.top = 0.79541015625f;
    glyphs[72].atlas.left = 82.5f;
    glyphs[72].atlas.bottom = 45.5f;
    glyphs[72].atlas.right = 93.5f;
    glyphs[72].atlas.top = 59.5f;
    
    glyphs[73].unicode = 73;
    glyphs[73].advance = 0.27783203125f;
    glyphs[73].plane.left = 0.015625f;
    glyphs[73].plane.bottom = -0.07958984375f;
    glyphs[73].plane.right = 0.265625f;
    glyphs[73].plane.top = 0.79541015625f;
    glyphs[73].atlas.left = 77.5f;
    glyphs[73].atlas.bottom = 45.5f;
    glyphs[73].atlas.right = 81.5f;
    glyphs[73].atlas.top = 59.5f;
    
    glyphs[74].unicode = 74;
    glyphs[74].advance = 0.5f;
    glyphs[74].plane.left = -0.055700913915094324f;
    glyphs[74].plane.bottom = -0.085693359375f;
    glyphs[74].plane.right = 0.50679908608490565f;
    glyphs[74].plane.top = 0.789306640625f;
    glyphs[74].atlas.left = 114.5f;
    glyphs[74].atlas.bottom = 75.5f;
    glyphs[74].atlas.right = 123.5f;
    glyphs[74].atlas.top = 89.5f;
    
    glyphs[75].unicode = 75;
    glyphs[75].advance = 0.6669921875f;
    glyphs[75].plane.left = -0.005859375f;
    glyphs[75].plane.bottom = -0.07958984375f;
    glyphs[75].plane.right = 0.744140625f;
    glyphs[75].plane.top = 0.79541015625f;
    glyphs[75].atlas.left = 50.5f;
    glyphs[75].atlas.bottom = 45.5f;
    glyphs[75].atlas.right = 62.5f;
    glyphs[75].atlas.top = 59.5f;
    
    glyphs[76].unicode = 76;
    glyphs[76].advance = 0.55615234375f;
    glyphs[76].plane.left = -0.015625f;
    glyphs[76].plane.bottom = -0.07958984375f;
    glyphs[76].plane.right = 0.609375f;
    glyphs[76].plane.top = 0.79541015625f;
    glyphs[76].atlas.left = 39.5f;
    glyphs[76].atlas.bottom = 45.5f;
    glyphs[76].atlas.right = 49.5f;
    glyphs[76].atlas.top = 59.5f;
    
    glyphs[77].unicode = 77;
    glyphs[77].advance = 0.8330078125f;
    glyphs[77].plane.left = 0.009521484375f;
    glyphs[77].plane.bottom = -0.07958984375f;
    glyphs[77].plane.right = 0.822021484375f;
    glyphs[77].plane.top = 0.79541015625f;
    glyphs[77].atlas.left = 25.5f;
    glyphs[77].atlas.bottom = 45.5f;
    glyphs[77].atlas.right = 38.5f;
    glyphs[77].atlas.top = 59.5f;
    
    glyphs[78].unicode = 78;
    glyphs[78].advance = 0.72216796875f;
    glyphs[78].plane.left = -0.016845703125f;
    glyphs[78].plane.bottom = -0.07958984375f;
    glyphs[78].plane.right = 0.733154296875f;
    glyphs[78].plane.top = 0.79541015625f;
    glyphs[78].atlas.left = 12.5f;
    glyphs[78].atlas.bottom = 45.5f;
    glyphs[78].atlas.right = 24.5f;
    glyphs[78].atlas.top = 59.5f;
    
    glyphs[79].unicode = 79;
    glyphs[79].advance = 0.77783203125f;
    glyphs[79].plane.left = -0.015625f;
    glyphs[79].plane.bottom = -0.079345703125f;
    glyphs[79].plane.right = 0.796875f;
    glyphs[79].plane.top = 0.795654296875f;
    glyphs[79].atlas.left = 25.5f;
    glyphs[79].atlas.bottom = 30.5f;
    glyphs[79].atlas.right = 38.5f;
    glyphs[79].atlas.top = 44.5f;
    
    glyphs[80].unicode = 80;
    glyphs[80].advance = 0.6669921875f;
    glyphs[80].plane.left = 0.006591796875f;
    glyphs[80].plane.bottom = -0.07958984375f;
    glyphs[80].plane.right = 0.694091796875f;
    glyphs[80].plane.top = 0.79541015625f;
    glyphs[80].atlas.left = 0.5f;
    glyphs[80].atlas.bottom = 45.5f;
    glyphs[80].atlas.right = 11.5f;
    glyphs[80].atlas.top = 59.5f;
    
    glyphs[81].unicode = 81;
    glyphs[81].advance = 0.77783203125f;
    glyphs[81].plane.left = -0.04541015625f;
    glyphs[81].plane.bottom = -0.13232421875f;
    glyphs[81].plane.right = 0.82958984375f;
    glyphs[81].plane.top = 0.80517578125f;
    glyphs[81].atlas.left = 101.5f;
    glyphs[81].atlas.bottom = 108.5f;
    glyphs[81].atlas.right = 115.5f;
    glyphs[81].atlas.top = 123.5f;
    
    glyphs[82].unicode = 82;
    glyphs[82].advance = 0.72216796875f;
    glyphs[82].plane.left = -0.01220703125f;
    glyphs[82].plane.bottom = -0.07958984375f;
    glyphs[82].plane.right = 0.80029296875f;
    glyphs[82].plane.top = 0.79541015625f;
    glyphs[82].atlas.left = 85.5f;
    glyphs[82].atlas.bottom = 60.5f;
    glyphs[82].atlas.right = 98.5f;
    glyphs[82].atlas.top = 74.5f;
    
    glyphs[83].unicode = 83;
    glyphs[83].advance = 0.6669921875f;
    glyphs[83].plane.left = -0.045166015625f;
    glyphs[83].plane.bottom = -0.07958984375f;
    glyphs[83].plane.right = 0.704833984375f;
    glyphs[83].plane.top = 0.79541015625f;
    glyphs[83].atlas.left = 72.5f;
    glyphs[83].atlas.bottom = 60.5f;
    glyphs[83].atlas.right = 84.5f;
    glyphs[83].atlas.top = 74.5f;
    
    glyphs[84].unicode = 84;
    glyphs[84].advance = 0.61083984375f;
    glyphs[84].plane.left = -0.06787109375f;
    glyphs[84].plane.bottom = -0.07958984375f;
    glyphs[84].plane.right = 0.68212890625f;
    glyphs[84].plane.top = 0.79541015625f;
    glyphs[84].atlas.left = 59.5f;
    glyphs[84].atlas.bottom = 60.5f;
    glyphs[84].atlas.right = 71.5f;
    glyphs[84].atlas.top = 74.5f;
    
    glyphs[85].unicode = 85;
    glyphs[85].advance = 0.72216796875f;
    glyphs[85].plane.left = -0.014892578125f;
    glyphs[85].plane.bottom = -0.085693359375f;
    glyphs[85].plane.right = 0.735107421875f;
    glyphs[85].plane.top = 0.789306640625f;
    glyphs[85].atlas.left = 46.5f;
    glyphs[85].atlas.bottom = 60.5f;
    glyphs[85].atlas.right = 58.5f;
    glyphs[85].atlas.top = 74.5f;
    
    glyphs[86].unicode = 86;
    glyphs[86].advance = 0.6669921875f;
    glyphs[86].plane.left = -0.074462890625f;
    glyphs[86].plane.bottom = -0.07958984375f;
    glyphs[86].plane.right = 0.738037109375f;
    glyphs[86].plane.top = 0.79541015625f;
    glyphs[86].atlas.left = 32.5f;
    glyphs[86].atlas.bottom = 60.5f;
    glyphs[86].atlas.right = 45.5f;
    glyphs[86].atlas.top = 74.5f;
    
    glyphs[87].unicode = 87;
    glyphs[87].advance = 0.94384765625f;
    glyphs[87].plane.left = -0.058837890625f;
    glyphs[87].plane.bottom = -0.07958984375f;
    glyphs[87].plane.right = 1.003662109375f;
    glyphs[87].plane.top = 0.79541015625f;
    glyphs[87].atlas.left = 14.5f;
    glyphs[87].atlas.bottom = 60.5f;
    glyphs[87].atlas.right = 31.5f;
    glyphs[87].atlas.top = 74.5f;
    
    glyphs[88].unicode = 88;
    glyphs[88].advance = 0.6669921875f;
    glyphs[88].plane.left = -0.07373046875f;
    glyphs[88].plane.bottom = -0.07958984375f;
    glyphs[88].plane.right = 0.73876953125f;
    glyphs[88].plane.top = 0.79541015625f;
    glyphs[88].atlas.left = 63.5f;
    glyphs[88].atlas.bottom = 45.5f;
    glyphs[88].atlas.right = 76.5f;
    glyphs[88].atlas.top = 59.5f;
    
    glyphs[89].unicode = 89;
    glyphs[89].advance = 0.6669921875f;
    glyphs[89].plane.left = -0.0751953125f;
    glyphs[89].plane.bottom = -0.07958984375f;
    glyphs[89].plane.right = 0.7373046875f;
    glyphs[89].plane.top = 0.79541015625f;
    glyphs[89].atlas.left = 0.5f;
    glyphs[89].atlas.bottom = 60.5f;
    glyphs[89].atlas.right = 13.5f;
    glyphs[89].atlas.top = 74.5f;
    
    glyphs[90].unicode = 90;
    glyphs[90].advance = 0.61083984375f;
    glyphs[90].plane.left = -0.072021484375f;
    glyphs[90].plane.bottom = -0.07958984375f;
    glyphs[90].plane.right = 0.677978515625f;
    glyphs[90].plane.top = 0.79541015625f;
    glyphs[90].atlas.left = 101.5f;
    glyphs[90].atlas.bottom = 75.5f;
    glyphs[90].atlas.right = 113.5f;
    glyphs[90].atlas.top = 89.5f;
    
    glyphs[91].unicode = 91;
    glyphs[91].advance = 0.27783203125f;
    glyphs[91].plane.left = -0.022705078125f;
    glyphs[91].plane.bottom = -0.272705078125f;
    glyphs[91].plane.right = 0.352294921875f;
    glyphs[91].plane.top = 0.789794921875f;
    glyphs[91].atlas.left = 64.5f;
    glyphs[91].atlas.bottom = 106.5f;
    glyphs[91].atlas.right = 70.5f;
    glyphs[91].atlas.top = 123.5f;
    
    glyphs[92].unicode = 92;
    glyphs[92].advance = 0.27783203125f;
    glyphs[92].plane.left = -0.079833984375f;
    glyphs[92].plane.bottom = -0.07958984375f;
    glyphs[92].plane.right = 0.357666015625f;
    glyphs[92].plane.top = 0.79541015625f;
    glyphs[92].atlas.left = 82.5f;
    glyphs[92].atlas.bottom = 75.5f;
    glyphs[92].atlas.right = 89.5f;
    glyphs[92].atlas.top = 89.5f;
    
    glyphs[93].unicode = 93;
    glyphs[93].advance = 0.27783203125f;
    glyphs[93].plane.left = -0.071533203125f;
    glyphs[93].plane.bottom = -0.272705078125f;
    glyphs[93].plane.right = 0.303466796875f;
    glyphs[93].plane.top = 0.789794921875f;
    glyphs[93].atlas.left = 71.5f;
    glyphs[93].atlas.bottom = 106.5f;
    glyphs[93].atlas.right = 77.5f;
    glyphs[93].atlas.top = 123.5f;
    
    glyphs[94].unicode = 94;
    glyphs[94].advance = 0.46923828125f;
    glyphs[94].plane.left = -0.046630859375f;
    glyphs[94].plane.bottom = 0.251220703125f;
    glyphs[94].plane.right = 0.515869140625f;
    glyphs[94].plane.top = 0.813720703125f;
    glyphs[94].atlas.left = 114.5f;
    glyphs[94].atlas.bottom = 5.5f;
    glyphs[94].atlas.right = 123.5f;
    glyphs[94].atlas.top = 14.5f;
    
    glyphs[95].unicode = 95;
    glyphs[95].advance = 0.55615234375f;
    glyphs[95].plane.left = -0.098876953125f;
    glyphs[95].plane.bottom = -0.2919921875f;
    glyphs[95].plane.right = 0.651123046875f;
    glyphs[95].plane.top = -0.0419921875f;
    glyphs[95].atlas.left = 100.5f;
    glyphs[95].atlas.bottom = 25.5f;
    glyphs[95].atlas.right = 112.5f;
    glyphs[95].atlas.top = 29.5f;
    
    glyphs[96].unicode = 96;
    glyphs[96].advance = 0.3330078125f;
    glyphs[96].plane.left = -0.02099609375f;
    glyphs[96].plane.bottom = 0.4951171875f;
    glyphs[96].plane.right = 0.29150390625f;
    glyphs[96].plane.top = 0.8076171875f;
    glyphs[96].atlas.left = 86.5f;
    glyphs[96].atlas.bottom = 24.5f;
    glyphs[96].atlas.right = 91.5f;
    glyphs[96].atlas.top = 29.5f;
    
    glyphs[97].unicode = 97;
    glyphs[97].advance = 0.55615234375f;
    glyphs[97].plane.left = -0.03759765625f;
    glyphs[97].plane.bottom = -0.08447265625f;
    glyphs[97].plane.right = 0.58740234375f;
    glyphs[97].plane.top = 0.60302734375f;
    glyphs[97].atlas.left = 82.5f;
    glyphs[97].atlas.bottom = 3.5f;
    glyphs[97].atlas.right = 92.5f;
    glyphs[97].atlas.top = 14.5f;
    
    glyphs[98].unicode = 98;
    glyphs[98].advance = 0.55615234375f;
    glyphs[98].plane.left = -0.022216796875f;
    glyphs[98].plane.bottom = -0.08544921875f;
    glyphs[98].plane.right = 0.602783203125f;
    glyphs[98].plane.top = 0.78955078125f;
    glyphs[98].atlas.left = 19.5f;
    glyphs[98].atlas.bottom = 75.5f;
    glyphs[98].atlas.right = 29.5f;
    glyphs[98].atlas.top = 89.5f;
    
    glyphs[99].unicode = 99;
    glyphs[99].advance = 0.5f;
    glyphs[99].plane.left = -0.047607421875f;
    glyphs[99].plane.bottom = -0.08447265625f;
    glyphs[99].plane.right = 0.577392578125f;
    glyphs[99].plane.top = 0.60302734375f;
    glyphs[99].atlas.left = 71.5f;
    glyphs[99].atlas.bottom = 3.5f;
    glyphs[99].atlas.right = 81.5f;
    glyphs[99].atlas.top = 14.5f;
    
    glyphs[100].unicode = 100;
    glyphs[100].advance = 0.55615234375f;
    glyphs[100].plane.left = -0.053466796875f;
    glyphs[100].plane.bottom = -0.08544921875f;
    glyphs[100].plane.right = 0.571533203125f;
    glyphs[100].plane.top = 0.78955078125f;
    glyphs[100].atlas.left = 0.5f;
    glyphs[100].atlas.bottom = 75.5f;
    glyphs[100].atlas.right = 10.5f;
    glyphs[100].atlas.top = 89.5f;
    
    glyphs[101].unicode = 101;
    glyphs[101].advance = 0.55615234375f;
    glyphs[101].plane.left = -0.036865234375f;
    glyphs[101].plane.bottom = -0.08447265625f;
    glyphs[101].plane.right = 0.588134765625f;
    glyphs[101].plane.top = 0.60302734375f;
    glyphs[101].atlas.left = 103.5f;
    glyphs[101].atlas.bottom = 3.5f;
    glyphs[101].atlas.right = 113.5f;
    glyphs[101].atlas.top = 14.5f;
    
    glyphs[102].unicode = 102;
    glyphs[102].advance = 0.27783203125f;
    glyphs[102].plane.left = -0.057861328125f;
    glyphs[102].plane.bottom = -0.073486328125f;
    glyphs[102].plane.right = 0.379638671875f;
    glyphs[102].plane.top = 0.801513671875f;
    glyphs[102].atlas.left = 101.5f;
    glyphs[102].atlas.bottom = 90.5f;
    glyphs[102].atlas.right = 108.5f;
    glyphs[102].atlas.top = 104.5f;
    
    glyphs[103].unicode = 103;
    glyphs[103].advance = 0.55615234375f;
    glyphs[103].plane.left = -0.0517578125f;
    glyphs[103].plane.bottom = -0.277587890625f;
    glyphs[103].plane.right = 0.5732421875f;
    glyphs[103].plane.top = 0.597412109375f;
    glyphs[103].atlas.left = 90.5f;
    glyphs[103].atlas.bottom = 90.5f;
    glyphs[103].atlas.right = 100.5f;
    glyphs[103].atlas.top = 104.5f;
    
    glyphs[104].unicode = 104;
    glyphs[104].advance = 0.55615234375f;
    glyphs[104].plane.left = -0.004150390625f;
    glyphs[104].plane.bottom = -0.07958984375f;
    glyphs[104].plane.right = 0.558349609375f;
    glyphs[104].plane.top = 0.79541015625f;
    glyphs[104].atlas.left = 80.5f;
    glyphs[104].atlas.bottom = 90.5f;
    glyphs[104].atlas.right = 89.5f;
    glyphs[104].atlas.top = 104.5f;
    
    glyphs[105].unicode = 105;
    glyphs[105].advance = 0.22216796875f;
    glyphs[105].plane.left = -0.0146484375f;
    glyphs[105].plane.bottom = -0.07958984375f;
    glyphs[105].plane.right = 0.2353515625f;
    glyphs[105].plane.top = 0.79541015625f;
    glyphs[105].atlas.left = 75.5f;
    glyphs[105].atlas.bottom = 90.5f;
    glyphs[105].atlas.right = 79.5f;
    glyphs[105].atlas.top = 104.5f;
    
    glyphs[106].unicode = 106;
    glyphs[106].advance = 0.22216796875f;
    glyphs[106].plane.left = -0.1337890625f;
    glyphs[106].plane.bottom = -0.278564453125f;
    glyphs[106].plane.right = 0.2412109375f;
    glyphs[106].plane.top = 0.783935546875f;
    glyphs[106].atlas.left = 78.5f;
    glyphs[106].atlas.bottom = 106.5f;
    glyphs[106].atlas.right = 84.5f;
    glyphs[106].atlas.top = 123.5f;
    
    glyphs[107].unicode = 107;
    glyphs[107].advance = 0.5f;
    glyphs[107].plane.left = 0.0f;
    glyphs[107].plane.bottom = -0.07958984375f;
    glyphs[107].plane.right = 0.5625f;
    glyphs[107].plane.top = 0.79541015625f;
    glyphs[107].atlas.left = 54.5f;
    glyphs[107].atlas.bottom = 90.5f;
    glyphs[107].atlas.right = 63.5f;
    glyphs[107].atlas.top = 104.5f;
    
    glyphs[108].unicode = 108;
    glyphs[108].advance = 0.22216796875f;
    glyphs[108].plane.left = -0.01708984375f;
    glyphs[108].plane.bottom = -0.07958984375f;
    glyphs[108].plane.right = 0.23291015625f;
    glyphs[108].plane.top = 0.79541015625f;
    glyphs[108].atlas.left = 49.5f;
    glyphs[108].atlas.bottom = 90.5f;
    glyphs[108].atlas.right = 53.5f;
    glyphs[108].atlas.top = 104.5f;
    
    glyphs[109].unicode = 109;
    glyphs[109].advance = 0.8330078125f;
    glyphs[109].plane.left = -0.020263671875f;
    glyphs[109].plane.bottom = -0.07861328125f;
    glyphs[109].plane.right = 0.854736328125f;
    glyphs[109].plane.top = 0.60888671875f;
    glyphs[109].atlas.left = 109.5f;
    glyphs[109].atlas.bottom = 93.5f;
    glyphs[109].atlas.right = 123.5f;
    glyphs[109].atlas.top = 104.5f;
    
    glyphs[110].unicode = 110;
    glyphs[110].advance = 0.55615234375f;
    glyphs[110].plane.left = -0.004638671875f;
    glyphs[110].plane.bottom = -0.07861328125f;
    glyphs[110].plane.right = 0.557861328125f;
    glyphs[110].plane.top = 0.60888671875f;
    glyphs[110].atlas.left = 50.5f;
    glyphs[110].atlas.bottom = 3.5f;
    glyphs[110].atlas.right = 59.5f;
    glyphs[110].atlas.top = 14.5f;
    
    glyphs[111].unicode = 111;
    glyphs[111].advance = 0.55615234375f;
    glyphs[111].plane.left = -0.036376953125f;
    glyphs[111].plane.bottom = -0.08447265625f;
    glyphs[111].plane.right = 0.588623046875f;
    glyphs[111].plane.top = 0.60302734375f;
    glyphs[111].atlas.left = 5.5f;
    glyphs[111].atlas.bottom = 3.5f;
    glyphs[111].atlas.right = 15.5f;
    glyphs[111].atlas.top = 14.5f;
    
    glyphs[112].unicode = 112;
    glyphs[112].advance = 0.55615234375f;
    glyphs[112].plane.left = -0.021484375f;
    glyphs[112].plane.bottom = -0.271728515625f;
    glyphs[112].plane.right = 0.603515625f;
    glyphs[112].plane.top = 0.603271484375f;
    glyphs[112].atlas.left = 5.5f;
    glyphs[112].atlas.bottom = 90.5f;
    glyphs[112].atlas.right = 15.5f;
    glyphs[112].atlas.top = 104.5f;
    
    glyphs[113].unicode = 113;
    glyphs[113].advance = 0.55615234375f;
    glyphs[113].plane.left = -0.052734375f;
    glyphs[113].plane.bottom = -0.271728515625f;
    glyphs[113].plane.right = 0.572265625f;
    glyphs[113].plane.top = 0.603271484375f;
    glyphs[113].atlas.left = 64.5f;
    glyphs[113].atlas.bottom = 90.5f;
    glyphs[113].atlas.right = 74.5f;
    glyphs[113].atlas.top = 104.5f;
    
    glyphs[114].unicode = 114;
    glyphs[114].advance = 0.3330078125f;
    glyphs[114].plane.left = -0.012939453125f;
    glyphs[114].plane.bottom = -0.07861328125f;
    glyphs[114].plane.right = 0.424560546875f;
    glyphs[114].plane.top = 0.60888671875f;
    glyphs[114].atlas.left = 16.5f;
    glyphs[114].atlas.bottom = 3.5f;
    glyphs[114].atlas.right = 23.5f;
    glyphs[114].atlas.top = 14.5f;
    
    glyphs[115].unicode = 115;
    glyphs[115].advance = 0.5f;
    glyphs[115].plane.left = -0.03515625f;
    glyphs[115].plane.bottom = -0.08447265625f;
    glyphs[115].plane.right = 0.52734375f;
    glyphs[115].plane.top = 0.60302734375f;
    glyphs[115].atlas.left = 24.5f;
    glyphs[115].atlas.bottom = 3.5f;
    glyphs[115].atlas.right = 33.5f;
    glyphs[115].atlas.top = 14.5f;
    
    glyphs[116].unicode = 116;
    glyphs[116].advance = 0.27783203125f;
    glyphs[116].plane.left = -0.07470703125f;
    glyphs[116].plane.bottom = -0.091064453125f;
    glyphs[116].plane.right = 0.36279296875f;
    glyphs[116].plane.top = 0.783935546875f;
    glyphs[116].atlas.left = 28.5f;
    glyphs[116].atlas.bottom = 90.5f;
    glyphs[116].atlas.right = 35.5f;
    glyphs[116].atlas.top = 104.5f;
    
    glyphs[117].unicode = 117;
    glyphs[117].advance = 0.55615234375f;
    glyphs[117].plane.left = -0.007080078125f;
    glyphs[117].plane.bottom = -0.09033203125f;
    glyphs[117].plane.right = 0.555419921875f;
    glyphs[117].plane.top = 0.59716796875f;
    glyphs[117].atlas.left = 93.5f;
    glyphs[117].atlas.bottom = 3.5f;
    glyphs[117].atlas.right = 102.5f;
    glyphs[117].atlas.top = 14.5f;
    
    glyphs[118].unicode = 118;
    glyphs[118].advance = 0.5f;
    glyphs[118].plane.left = -0.06201171875f;
    glyphs[118].plane.bottom = -0.08447265625f;
    glyphs[118].plane.right = 0.56298828125f;
    glyphs[118].plane.top = 0.60302734375f;
    glyphs[118].atlas.left = 39.5f;
    glyphs[118].atlas.bottom = 3.5f;
    glyphs[118].atlas.right = 49.5f;
    glyphs[118].atlas.top = 14.5f;
    
    glyphs[119].unicode = 119;
    glyphs[119].advance = 0.72216796875f;
    glyphs[119].plane.left = -0.078857421875f;
    glyphs[119].plane.bottom = -0.08447265625f;
    glyphs[119].plane.right = 0.796142578125f;
    glyphs[119].plane.top = 0.60302734375f;
    glyphs[119].atlas.left = 109.5f;
    glyphs[119].atlas.bottom = 33.5f;
    glyphs[119].atlas.right = 123.5f;
    glyphs[119].atlas.top = 44.5f;
    
    glyphs[120].unicode = 120;
    glyphs[120].advance = 0.5f;
    glyphs[120].plane.left = -0.0625f;
    glyphs[120].plane.bottom = -0.08447265625f;
    glyphs[120].plane.right = 0.5625f;
    glyphs[120].plane.top = 0.60302734375f;
    glyphs[120].atlas.left = 60.5f;
    glyphs[120].atlas.bottom = 3.5f;
    glyphs[120].atlas.right = 70.5f;
    glyphs[120].atlas.top = 14.5f;
    
    glyphs[121].unicode = 121;
    glyphs[121].advance = 0.5f;
    glyphs[121].plane.left = -0.058837890625f;
    glyphs[121].plane.bottom = -0.283447265625f;
    glyphs[121].plane.right = 0.566162109375f;
    glyphs[121].plane.top = 0.591552734375f;
    glyphs[121].atlas.left = 39.5f;
    glyphs[121].atlas.bottom = 30.5f;
    glyphs[121].atlas.right = 49.5f;
    glyphs[121].atlas.top = 44.5f;
    
    glyphs[122].unicode = 122;
    glyphs[122].advance = 0.5f;
    glyphs[122].plane.left = -0.0634765625f;
    glyphs[122].plane.bottom = -0.08447265625f;
    glyphs[122].plane.right = 0.5615234375f;
    glyphs[122].plane.top = 0.60302734375f;
    glyphs[122].atlas.left = 11.5f;
    glyphs[122].atlas.bottom = 18.5f;
    glyphs[122].atlas.right = 21.5f;
    glyphs[122].atlas.top = 29.5f;
    
    glyphs[123].unicode = 123;
    glyphs[123].advance = 0.333984375f;
    glyphs[123].plane.left = -0.049560546875f;
    glyphs[123].plane.bottom = -0.3037109375f;
    glyphs[123].plane.right = 0.387939453125f;
    glyphs[123].plane.top = 0.8212890625f;
    glyphs[123].atlas.left = 27.5f;
    glyphs[123].atlas.bottom = 105.5f;
    glyphs[123].atlas.right = 34.5f;
    glyphs[123].atlas.top = 123.5f;
    
    glyphs[124].unicode = 124;
    glyphs[124].advance = 0.259765625f;
    glyphs[124].plane.left = 0.005126953125f;
    glyphs[124].plane.bottom = -0.3037109375f;
    glyphs[124].plane.right = 0.255126953125f;
    glyphs[124].plane.top = 0.8212890625f;
    glyphs[124].atlas.left = 22.5f;
    glyphs[124].atlas.bottom = 105.5f;
    glyphs[124].atlas.right = 26.5f;
    glyphs[124].atlas.top = 123.5f;
    
    glyphs[125].unicode = 125;
    glyphs[125].advance = 0.333984375f;
    glyphs[125].plane.left = -0.054443359375f;
    glyphs[125].plane.bottom = -0.3037109375f;
    glyphs[125].plane.right = 0.383056640625f;
    glyphs[125].plane.top = 0.8212890625f;
    glyphs[125].atlas.left = 14.5f;
    glyphs[125].atlas.bottom = 105.5f;
    glyphs[125].atlas.right = 21.5f;
    glyphs[125].atlas.top = 123.5f;
    
    glyphs[126].unicode = 126;
    glyphs[126].advance = 0.583984375f;
    glyphs[126].plane.left = -0.020263671875f;
    glyphs[126].plane.bottom = 0.19580078125f;
    glyphs[126].plane.right = 0.604736328125f;
    glyphs[126].plane.top = 0.50830078125f;
    glyphs[126].atlas.left = 75.5f;
    glyphs[126].atlas.bottom = 24.5f;
    glyphs[126].atlas.right = 85.5f;
    glyphs[126].atlas.top = 29.5f;
    
    while (!hy_window_should_close(&window)) {
        float currTime = hy_timer_get_milliseconds();
        float dt = currTime - lastTime;
        
        HyColor hcbg = hex_to_HyColor(bg0);
        HY_SetClearColorCmd(&hcbg);
        HY_ClearCmd();
        
        HyRenderer2DStats stats = hy_renderer2d_get_stats();
        
        hy_renderer2d_reset_stats();
        hy_renderer2d_begin_scene(&camera2D);
        {
            local_persist float cpuLoad = 0.0f;
            local_persist float currCpuLoad = 0.0f;
            
            cpuLoad = (float)hy_get_cpu_load();
            currCpuLoad += (cpuLoad - currCpuLoad) * (dt / 1000.0f);
            
            // Caption
            draw_quad_2c((vec2){0.0f, (float)window.height - HY_EDITOR_CAPTION_H}, (vec2){window.width, HY_EDITOR_CAPTION_H}, hex_to_HyColor(bg1));
            draw_quad_2tc(12.0f, (float)window.height - HY_EDITOR_CAPTION_H + 2, (vec2){ 24, 24 }, folderTexture, hex_to_HyColor(fg));
            draw_quad_2c((vec2){window.width - HY_EDITOR_CAPTION_W, (float)window.height - HY_EDITOR_CAPTION_H}, (vec2){HY_EDITOR_CAPTION_W, HY_EDITOR_CAPTION_H}, hex_to_HyColor(red1));
            draw_quad_2c((vec2){window.width - HY_EDITOR_CAPTION_W * 2, (float)window.height - HY_EDITOR_CAPTION_H}, (vec2){HY_EDITOR_CAPTION_W, HY_EDITOR_CAPTION_H}, hex_to_HyColor(bg0));
            draw_quad_2c((vec2){window.width - HY_EDITOR_CAPTION_W * 3, (float)window.height - HY_EDITOR_CAPTION_H}, (vec2){HY_EDITOR_CAPTION_W, HY_EDITOR_CAPTION_H}, hex_to_HyColor(bg0));
            draw_debug_text("Hyper", 500.0f, (float)window.height - HY_EDITOR_CAPTION_H+ 4, hex_to_HyColor(fg));
            
            // Text content
            draw_text(glyphs, testFile->data, 312.0f, (float)window.height - 16 - 12 - HY_EDITOR_CAPTION_H, hex_to_HyColor(fg));
            
            // Folder icon
            draw_quad_2tc(12.0f, (float)window.height - 16 - 12 - HY_EDITOR_CAPTION_H, (vec2){ 16, 16 }, folderTexture, hex_to_HyColor(fg));
            
            // Project tree
            draw_debug_text("Hyped", 30.0f, (float)window.height - 16 - 12 - HY_EDITOR_CAPTION_H, hex_to_HyColor(fg));
            draw_debug_text("src", 24.0f, (float)window.height - 16 * 2 - 12 - HY_EDITOR_CAPTION_H, hex_to_HyColor(fg));
            
            // Status bar
            draw_quad_2c((vec2){0.0f, 0.0f}, (vec2){window.width, HY_EDITOR_STATUS_H}, hex_to_HyColor(bg1));
            draw_quad_2tc(12.0f, 2, (vec2){ 16, 16 }, folderTexture, hex_to_HyColor(fg));
            draw_debug_text("master", 30.0f, 2.0f, hex_to_HyColor(fg));
            draw_debug_text("0", 150.0f, 2.0f, hex_to_HyColor(fg));
            draw_debug_text("0", 190.0f, 2.0f, hex_to_HyColor(fg));
#if 0
            // Debug info
            char glInfo[256] = {0};
            char drawInfo[256] = {0};
            char cpuInfo[256] = {0};
            snprintf(glInfo, 256,
                     "Vendor         : %s\nRenderer       : %s\nOpenGL version : "
                     "%s\nGLSL version   : %s",
                     window.glVendor, window.glRenderer, window.glVersion, window.glGLSL);
            snprintf(drawInfo, 256, "Renderer Draws: %d Quads: %d", stats.drawCount, stats.quadCount);
            snprintf(cpuInfo, 256, "CPU Load: %f\nFrame Time: %f", currCpuLoad, dt);
            draw_quad_2c((vec2){0.0f, (float)window.height - 300.0f}, (vec2){600.0f, 300.0f}, hex_to_HyColor(bg));
            draw_debug_text(glInfo, 12.0f, window.height - 28.0f, hex_to_HyColor(fg));
            draw_debug_text(drawInfo, 12.0f, window.height - 208.0f, hex_to_HyColor(fg));
            draw_debug_text(cpuInfo, 12.0f, window.height - 258.0f, hex_to_HyColor(fg));
#endif
        }
        hy_renderer2d_end_scene();
        
        hy_swap_buffers(&window);
        
        hy_poll_events(&window);
        
        lastTime = currTime;
        
        Sleep(1);
    }
    
    hy_window_destroy(&window);
    
    ExitProcess(0);
}

// Subsystem:windows
int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow)
{
    // Read cmd args.
    LPWSTR* argv;
    int     argc;
    int     i;
    
    // argv = CommandLineToArgvW(GetCommandLineW(), &argc);
    argv = CommandLineToArgvW(pCmdLine, &argc);
    if (NULL == argv) {
        HY_INFO("CommandLineToArgvW failed\n");
        return 0;
    } else {
        for (i = 0; i < argc; i++) {
            HY_INFO("%d: %ws\n", i, argv[i]);
        }
    }
    
    // TODO(alex): How tf do u use windows types?
    return main(argc, NULL);
    
    // LocalFree(argv); // Free memory allocated for CommandLineToArgvW arguments.
    
    // return result;
}