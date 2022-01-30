// This is an independent project of an individual developer. Dear PVS-Studio,
// please check it.

// PVS-Studio Static Code Analyzer for C, C++, C#, and Java:
// http://www.viva64.com

// TODO(alex): Indexed font rendering like 4coder.
// TODO(alex): Font rendering with stb_truetype.
// TODO(alex): Font caching (safe img to disk to avoid computing at startup).
// TODO(alex): What is the right way to add an icon without Visual Studio?

#define HY_EDITOR_CAPTION_W 46
#define HY_EDITOR_CAPTION_H 30
#define HY_EDITOR_STATUS_H  22

#include "hy_types.c"
#include "hy_assert.c"
#include "hy_log.c"
#include "hy_platform.h"

#pragma warning(push)
#pragma warning(disable : 4459) // declaration of 'identifier' hides global declaration
#pragma warning(disable : 4996) // This function or variable may be unsafe. (for ini.c in release) // TODO(alex): Fix

#include <cglm/cglm.h>
#include <cglm/cam.h>
#include <ini.c>

// OpenGL Loader
#define FGL_IMPLEMENTATION
#include <final_dynamic_opengl.h>

// STB Image
#define STB_IMAGE_IMPLEMENTATION
#define STBI_MALLOC(size)        hy_malloc(size)
#define STBI_REALLOC(ptr, newsz) hy_realloc(ptr, newsz)
#define STBI_FREE(ptr)           hy_free(ptr)
#include <stb_image.h>

// STB Image resize
#define STB_IMAGE_RESIZE_IMPLEMENTATION
#define STBIR_MALLOC(size, context) hy_malloc(size)
#define STBIR_FREE(ptr, context)    hy_free(ptr)
#include <stb_image_resize.h>

// STB Truetype
#define STB_TRUETYPE_IMPLEMENTATION
#define STBTT_MALLOC(size, context) hy_malloc(size)
#define STBTT_FREE(ptr, context)    hy_free(ptr)
#include <stb_truetype.h>

#pragma warning(pop)

global_variable int g_ShowDebugOverlay = false;

#include "hy_file.c"
#include "win32_platform.c"
#include "win32_renderer_ogl.c"
#include "hy_imgui.c"
#include "hy_config.c"

#include <git2.h>

global_variable HyCamera2D camera2D;

internal void SizeCallback(HyWindow* hyWindow, unsigned int width, unsigned int height)
{
    // TODO(alex): Remove when use framebuffer
    HyCamera2D_Resize(&camera2D, (float)width, (float)height, -1.0f, 1.0f);
    GL_CALL(glViewport(0, 0, width, height));
}

typedef struct {
    uint32_t changesCount;
    char**   paths;
} status_data;

int git_status_callback(const char* path, uint32_t status_flags, void* payload)
{
    status_data* repo_status_data = (status_data*)payload;
    
    size_t path_len = strlen(path);
    
    // Skip forders (we list files individually) and ignored files (from .gitignore)
    if (path[path_len - 1] == '/' || status_flags & GIT_STATUS_IGNORED) {
        return false;
    }
    
    HY_INFO("%d %s", repo_status_data->changesCount, path);
    repo_status_data->paths[repo_status_data->changesCount] = malloc(path_len * sizeof(char) + 1);
    memcpy(repo_status_data->paths[repo_status_data->changesCount], path, path_len);
    repo_status_data->paths[repo_status_data->changesCount][path_len] = '\0';
    repo_status_data->changesCount++;
    
    return false;
}

int hy_main(int argc, char* argv[])
{
    hy_log_init();
    hy_timer_init();
    HyConfig* config = hy_config_init();
    
    HyFile* testFile = hy_read_file("src/win32_hyper.c");
    
    HyWindow window = {0};
    hy_window_create_borderless(&window, config->startMode, "Hyped");
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
    
    // ==============================
    // Git test
    
    git_libgit2_init();
    
    // TODO(alex): Make a cross platform API.
    char* repo_path = "";
    if (argc > 1) {
        repo_path = argv[1];
    }
    
    // Check repo existence
    int error = git_repository_open_ext(NULL, repo_path, GIT_REPOSITORY_OPEN_NO_SEARCH, NULL);
    if (error == 0) {
        HY_INFO("Current directory is a repository.");
    } else if (error < 0) {
        const git_error* e = git_error_last();
        HY_ERROR("Error %d/%d: %s", error, e->klass, e->message);
    }
    
    status_data repo_status_data = {0};
    
    // Open repo
    git_repository* repo = NULL;
    error = git_repository_open(&repo, repo_path);
    if (error < 0) {
        const git_error* e = git_error_last();
        HY_ERROR("Error %d/%d: %s", error, e->klass, e->message);
    } else {
        repo_status_data.paths = malloc(sizeof(char*) * 300);
        error = git_status_foreach(repo, git_status_callback, &repo_status_data);
        
        if (error < 0) {
            const git_error* e = git_error_last();
            HY_ERROR("Error %d/%d: %s", error, e->klass, e->message);
        }
    }
    
    // ==============================
    
    hyui_init();
    
    while (!hy_window_should_close(&window)) {
        float currTime = hy_timer_get_milliseconds();
        float dt = currTime - lastTime;
        
        // Update
        hy_poll_events(&window);
        
        // Render
        HyColor hcbg = hex_to_HyColor(bg0_s);
        HY_SetClearColorCmd(&hcbg);
        HY_ClearCmd();
        
        HyRenderer2DStats stats = hy_renderer2d_get_stats();
        hy_renderer2d_reset_stats();
        
        hy_renderer2d_begin_scene(&camera2D);
        {
            local_persist float cpuLoad = 0.0f;
            local_persist float currCpuLoad = 0.0f;
            
            hyui_begin("Root", window.width, window.height);
            {
                hyui_text("Staged Changes");
                
                hyui_text("Changes");
                
                for (uint32_t i = 0; i < repo_status_data.changesCount; ++i) {
                    hyui_text(repo_status_data.paths[i]);
                }
                
                hyui_button("Test button");
                
                hyui_text("Icon buttons:");
                
                hyui_begin_row();
                {
                    hyui_icon_button(hyperIcon);
                    hyui_icon_button(gitIcon);
                    hyui_icon_button(editIcon);
                    hyui_icon_button(downloadIcon);
                    hyui_icon_button(uploadIcon);
                }
                hyui_end_row();
            }
            hyui_end();
            
            hyui_render();
            
            cpuLoad = (float)hy_get_cpu_load();
            currCpuLoad += (cpuLoad - currCpuLoad) * (dt / 1000.0f);
            
            // Text content
            if (testFile) {
                draw_debug_text(testFile->data, 312.0f, (float)window.height - 16 - 12 - HY_EDITOR_CAPTION_H + 210, hex_to_HyColor(fg));
            }
            
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
            draw_debug_text("-", p[0], p[1], hex_to_HyColor(fg));
            p[0] += FONT_SIZE * 1.5f;
            
            // Push
            draw_quad_2tc(p[0], p[1], (vec2){ FONT_SIZE, FONT_SIZE }, uploadIcon, hex_to_HyColor(fg));
            p[0] += FONT_SIZE;
            draw_debug_text("-", p[0], p[1], hex_to_HyColor(fg));
            p[0] += FONT_SIZE * 1.5f;
            
            // Edits
            char editsInfo[256] = {0};
            snprintf(editsInfo, 256, "%d", repo_status_data.changesCount);
            draw_quad_2tc(p[0], p[1], (vec2){ FONT_SIZE, FONT_SIZE }, editIcon, hex_to_HyColor(fg));
            p[0] += FONT_SIZE;
            draw_debug_text(editsInfo, p[0], p[1], hex_to_HyColor(fg));
            p[0] += FONT_SIZE * 2.0f;
            
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
            
            if (g_ShowDebugOverlay) {
                
                p[0] = 0;
                p[1] = window.height - FONT_SIZE * 2;
                draw_quad_2c((vec2){0.0f, (float)window.height - FONT_SIZE * 8 }, (vec2){600.0f, 300.0f}, HyDebugBg);
                draw_debug_text(glInfo, 12.0f, p[1], hex_to_HyColor(fg));
                p[1] -= FONT_SIZE * 4;
                draw_debug_text(drawInfo, 12.0f, p[1], hex_to_HyColor(fg));
                p[1] -= FONT_SIZE;
                draw_debug_text(cpuInfo, 12.0f, p[1], hex_to_HyColor(fg));
            }
        }
        hy_renderer2d_end_scene();
        
        hy_swap_buffers(&window);
        
        lastTime = currTime;
        
        hy_sleep(1);
    }
    
    git_repository_free(repo);
    git_libgit2_shutdown();
    
    hy_texture_destroy(restoreIcon);
    hy_window_destroy_borderless(&window);
    hy_config_deinit(config);
    
    return 0;
}
