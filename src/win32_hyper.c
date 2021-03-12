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
#define HY_EDITOR_STATUS_H 22

#include "hy_types.c"
#include "hy_assert.c"
#include "hy_log.c"
#include "hy_platform.h"

#pragma warning(push)
#pragma warning(disable : 4459) // declaration of 'identifier' hides global declaration
#pragma warning(disable : 4996) // declaration of 'identifier' hides global declaration

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

int hy_main(int argc, char* argv[])
{
    hy_log_init();
    hy_timer_init();
    
    HyConfig config = {0};
    config.startMode = HyWindowStartMode_Auto;
    
    HyFile* configFile = hy_read_file(".hypedrc");
    if (configFile) {
        ini_parse_string(configFile->data, configHandler, &config);
        HY_INFO("Config loaded from '.hypedrc'");
        HY_INFO("  - startMode=%d", config.startMode);
        HY_INFO("  - user=%s", config.user);
    } else {
        HY_ERROR(".hypedrc not found");
    }
    
    HyFile* testFile = hy_read_file("src/win32_hyper.c");
    
    HyWindow window = {0};
    hy_window_create_borderless(&window, config.startMode, "Hyped");
    hy_set_window_size_callback(&window, SizeCallback);
    
    if (!&window) { // ?
        MessageBox(NULL, "Failed to create window.", "Hyper", MB_ICONERROR);
        ExitProcess(0);
    }
    
    camera2D = HyCamera2D_Create((float)window.width, (float)window.height, -1, 1);
    
    hy_renderer2d_init();
    
    HyTexture* asciiTexture = hy_texture_create("assets/textures/Fira Code-9(18).png", HyTextureFilterMode_Linear);
    
    HyTexture* hyperIcon = hy_texture_create("assets/icons/hyper-logo-24.png", HyTextureFilterMode_Linear);
    
    HyTexture* gitIcon = hy_texture_create("assets/icons/git.png", HyTextureFilterMode_Linear);
    HyTexture* uploadIcon = hy_texture_create("assets/icons/upload.png", HyTextureFilterMode_Linear);
    HyTexture* downloadIcon = hy_texture_create("assets/icons/download.png", HyTextureFilterMode_Linear);
    HyTexture* editIcon = hy_texture_create("assets/icons/edit.png", HyTextureFilterMode_Linear);
    
    HyTexture* closeIcon = hy_texture_create("assets/icons/chrome-close.png", HyTextureFilterMode_Linear);
    HyTexture* restoreIcon = hy_texture_create("assets/icons/chrome-restore.png", HyTextureFilterMode_Linear);
    HyTexture* minimizeIcon = hy_texture_create("assets/icons/chrome-minimize.png", HyTextureFilterMode_Linear);
    HyTexture* maximizeIcon = hy_texture_create("assets/icons/chrome-maximize.png", HyTextureFilterMode_Linear);
    
    // TODO(alex): Move into renderer init struct
    g_renderer.asciiTexture = asciiTexture;
    
    float lastTime = 0.0f;
    
    GL_CALL(glEnable(GL_CULL_FACE));
    GL_CALL(glEnable(GL_BLEND));
    //GL_CALL(glEnable(GL_DEPTH_TEST));
    GL_CALL(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));
    
    while (!hy_window_should_close(&window)) {
        float currTime = hy_timer_get_milliseconds();
        float dt = currTime - lastTime;
        
        HyColor hcbg = hex_to_HyColor(bg0_s);
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
            
            // Text content
            if (testFile) {
                draw_debug_text(testFile->data, 312.0f, (float)window.height - 16 - 12 - HY_EDITOR_CAPTION_H + 210, hex_to_HyColor(fg));
            }
            
            // Folder icon
            draw_quad_2tc(12.0f, (float)window.height - 16 - 12 - HY_EDITOR_CAPTION_H, (vec2){ 16, 16 }, gitIcon, HyWhite);
            
            // Project tree
            draw_debug_text("Hyped", 30.0f, (float)window.height - 16 - 12 - HY_EDITOR_CAPTION_H, hex_to_HyColor(fg));
            draw_debug_text("src", 24.0f, (float)window.height - 16 * 2 - 12 - HY_EDITOR_CAPTION_H, hex_to_HyColor(fg));
            
            // Caption
            draw_quad_2c((vec3){ 0.0f, (float)window.height - HY_EDITOR_CAPTION_H }, (vec2){window.width, HY_EDITOR_CAPTION_H}, hex_to_HyColor(bg0_s));
            draw_quad_2tc(10.0f, (float)window.height - HY_EDITOR_CAPTION_H + 5, (vec2){ 20, 20 }, hyperIcon, HyWhite);
            draw_debug_text("Hyper", 34.0f, (float)window.height - HY_EDITOR_CAPTION_H + ((HY_EDITOR_CAPTION_H - FONT_SIZE) / 2.0f), hex_to_HyColor(fg));
            
            // Minimize button
            draw_quad_2c((vec2){ window.width - HY_EDITOR_CAPTION_W * 3, (float)window.height - HY_EDITOR_CAPTION_H },
                         (vec2){ HY_EDITOR_CAPTION_W, HY_EDITOR_CAPTION_H },
                         hex_to_HyColor(bg0_s));
            draw_quad_2tc(window.width - HY_EDITOR_CAPTION_W * 3, (float)window.height - HY_EDITOR_CAPTION_H,
                          (vec2){ HY_EDITOR_CAPTION_W, HY_EDITOR_CAPTION_H },
                          minimizeIcon, hex_to_HyColor(fg));
            
            // Maximize button
            draw_quad_2c((vec2){ window.width - HY_EDITOR_CAPTION_W * 2, (float)window.height - HY_EDITOR_CAPTION_H },
                         (vec2){ HY_EDITOR_CAPTION_W, HY_EDITOR_CAPTION_H },
                         hex_to_HyColor(bg0_s));
            draw_quad_2tc(window.width - HY_EDITOR_CAPTION_W * 2, (float)window.height - HY_EDITOR_CAPTION_H,
                          (vec2){ HY_EDITOR_CAPTION_W, HY_EDITOR_CAPTION_H },
                          maximizeIcon, hex_to_HyColor(fg));
            
            // Close button
            draw_quad_2c((vec2){ window.width - HY_EDITOR_CAPTION_W, (float)window.height - HY_EDITOR_CAPTION_H },
                         (vec2){ HY_EDITOR_CAPTION_W, HY_EDITOR_CAPTION_H },
                         hex_to_HyColor(bg0_s));
            draw_quad_2tc(window.width - HY_EDITOR_CAPTION_W, (float)window.height - HY_EDITOR_CAPTION_H,
                          (vec2){ HY_EDITOR_CAPTION_W, HY_EDITOR_CAPTION_H },
                          closeIcon, hex_to_HyColor(fg));
            
            // Status bar
            vec2 p = { 10.0f, (HY_EDITOR_STATUS_H - FONT_SIZE) / 2.0f - 2.0f };
            draw_quad_2c((vec2){0.0f, 0.0f}, (vec2){window.width, HY_EDITOR_STATUS_H}, hex_to_HyColor(bg0_s));
            draw_quad_2tc(p[0], p[1], (vec2){ FONT_SIZE, FONT_SIZE }, gitIcon, HyWhite);
            p[0] += FONT_SIZE;
            draw_debug_text("master", p[0], p[1], hex_to_HyColor(fg));
            p[0] += FONT_SIZE * 4;
            
            // Pull
            draw_quad_2tc(p[0], p[1], (vec2){ FONT_SIZE, FONT_SIZE }, downloadIcon, hex_to_HyColor(fg));
            p[0] += FONT_SIZE;
            draw_debug_text("0", p[0], p[1], hex_to_HyColor(fg));
            p[0] += FONT_SIZE * 1.5f;
            
            // Push
            draw_quad_2tc(p[0], p[1], (vec2){ FONT_SIZE, FONT_SIZE }, uploadIcon, hex_to_HyColor(fg));
            p[0] += FONT_SIZE;
            draw_debug_text("0", p[0], p[1], hex_to_HyColor(fg));
            p[0] += FONT_SIZE * 1.5f;
            
            // Edits
            draw_quad_2tc(p[0], p[1], (vec2){ FONT_SIZE, FONT_SIZE }, editIcon, hex_to_HyColor(fg));
            p[0] += FONT_SIZE;
            draw_debug_text("0", p[0], p[1], hex_to_HyColor(fg));
            p[0] += FONT_SIZE * 1.5f;
            
            // Debug info
            char glInfo[256] = {0};
            char drawInfo[256] = {0};
            char cpuInfo[256] = {0};
            snprintf(glInfo, 256,
                     "Vendor         : %s\nRenderer       : %s\nOpenGL version : "
                     "%s\nGLSL version   : %s",
                     window.glVendor, window.glRenderer, window.glVersion, window.glGLSL);
            snprintf(drawInfo, 256, "Draws: %d | Quads: %d", stats.drawCount, stats.quadCount);
            snprintf(cpuInfo, 256, "CPU Load: %2.0f%% | Frame Time: %3.1fms", currCpuLoad * 100.0f, dt);
            
            draw_debug_text(drawInfo, p[0], p[1], hex_to_HyColor(fg));
            p[0] += FONT_SIZE * 20.0f;
            draw_debug_text(cpuInfo, p[0], p[1], hex_to_HyColor(fg));
            p[0] += FONT_SIZE * 1.5f;
#if 0
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
    
    return 0;
}
