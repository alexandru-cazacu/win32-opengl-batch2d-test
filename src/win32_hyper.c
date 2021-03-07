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

#include "hy_types.c"
#include "hy_assert.c"
#include "hy_log.c"
#include "hy_platform.h"
#include "hy_time.c"

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
#include "win32_platform.c"
#include "win32_renderer_opengl.c"
#include "hy_ui.c"

// TODO(alex): What is the right way to add an icon without Visual Studio?

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
    HyColor hcbg = hex_to_HyColor(bg);
    HY_SetClearColorCmd(&hcbg);
    HY_ClearCmd();
    draw_debug_text("Resizing, you may want to keep drawing....", 20.0f, (float)height - 300, hex_to_HyColor(fg));
    hy_swap_buffers(hyWindow);
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
    HyTexture* asciiTexture = hy_texture_create("assets/textures/DejaVu Sans Mono.png", HyTextureFilterMode_Linear);
    HyTexture* folderTexture = hy_texture_create("assets/icons/folder.png", HyTextureFilterMode_Linear);
    
    // TODO(alex): Move into renderer init struct
    g_renderer.asciiTexture = asciiTexture;
    
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    float lastTime = 0.0f;
    
    while (!hy_window_should_close(&window)) {
        float currTime = hy_timer_get_milliseconds();
        float dt = currTime - lastTime;
        
        HyRenderer2DStats stats = hy_renderer2d_get_stats();
        
        hy_renderer2d_reset_stats();
        hy_renderer2d_begin_scene(&camera2D);
        {
            HyColor hcbg = hex_to_HyColor(bg);
            HY_SetClearColorCmd(&hcbg);
            HY_ClearCmd();
            
            local_persist float cpuLoad = 0.0f;
            local_persist float currCpuLoad = 0.0f;
            
            cpuLoad = (float)GetCPULoad();
            currCpuLoad += (cpuLoad - currCpuLoad) * (dt / 1000.0f);
            
            draw_quad_2c((vec2){0.0f, (float)window.height - 50.0f}, (vec2){window.width, 50.0f}, hex_to_HyColor(fg));
            
            draw_debug_text(testFile->data, 312.0f, (float)window.height - 16 - 12 - 50, hex_to_HyColor(fg));
            
            draw_quad_2tc(12.0f, (float)window.height - 16 - 12 - 50, (vec2){ 16, 16 }, folderTexture, hex_to_HyColor(fg));
            
            draw_debug_text("Hyped", 30.0f, (float)window.height - 16 - 12 - 50, hex_to_HyColor(fg));
            draw_debug_text("src", 24.0f, (float)window.height - 16 * 2 - 12 - 50, hex_to_HyColor(fg));
            
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
            
            hui_begin_row(); // App
            {
                hui_begin_row(); // App bar
                {
                    
                }
                hui_end_row();
                hui_begin_row(); // Content
                {
                    hui_begin_col(); // Activity Col
                    hui_end_col();
                    hui_begin_col(); // Explorer Col
                    hui_end_col();
                    hui_begin_col(); // Documents Col
                    hui_end_col();
                }
                hui_end_row();
                hui_begin_row(); // Status bar
                {
                    
                }
                hui_end_row();
            }
            hui_end_row();
        }
        hy_renderer2d_end_scene();
        
        hy_swap_buffers(&window);
        
        hy_poll_events(&window);
        
        lastTime = currTime;
        
        Sleep(10);
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