// This is an independent project of an individual developer. Dear PVS-Studio, please check it.

// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#define _WIN32_WINNT 0x0601 // Targets Windows 7 or later
#include <sdkddkver.h>

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
#define NOTEXTMETRIC
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

#include <dwmapi.h>
#include <shellapi.h>
#include <windows.h>
#include <windowsx.h>

#include "hy_log.c"
#include "hy_time.c"
#include "hy_types.c"
#include "resources.h"

#pragma warning(push)
#pragma warning(disable : 4204) // nonstandard extension used : non-constant aggregate initializer
#pragma warning(disable : 4996) // // TODO(alex): What error
#pragma warning(disable : 4459) // TODO(alex): What error
#pragma warning(disable : 4244) // TODO(alex): What error
#pragma warning(disable : 4267) // TODO(alex): What error
#pragma warning(disable : 4456) // TODO(alex): What error

#include <cglm/cam.h>
#include <cglm/cglm.h>
#include <ini.c>

#define FGL_IMPLEMENTATION
#include <final_dynamic_opengl.h>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#define STB_IMAGE_RESIZE_IMPLEMENTATION
#include <stb_image_resize.h>

#pragma warning(pop)

#include "hy_file.c"
#include "win32_renderer_opengl.c"
#include "win32_window.c"

// TODO(alex): What is the right way to add an icon without Visual Studio?

vec4  white = {1.0f, 1.0f, 1.0f, 1.0f};
vec4  bg0 = {0.157f, 0.157f, 0.157f, 1.0f};
vec4  bg1 = {0.235f, 0.22f, 0.212f, 1.0f};
vec4  red0 = {0.8f, 0.141f, 0.114f, 1.0f};
vec4  red1 = {0.984f, 0.286f, 0.204f, 1.0f};
vec4  green0 = {0.596f, 0.592f, 0.102f, 1.0f};
vec4  green1 = {0.722f, 0.733f, 0.149f, 1.0f};
vec4  blue0 = {0.271f, 0.522f, 0.533f, 1.0f};
vec4  blue1 = {0.514f, 0.647f, 0.596f, 1.0f};
float pad = 15.0f;
float border = 3.0f;
float ch = 35.0f;

typedef struct HyFrame HyFrame;

struct HyFrame {
  float width; // Internally or externally for
  float height;
  bool  fixedWidth;  // If true you must also set the width
  bool  fixedHeight; // If true you must also set the height

  float padding;

  vec4     color;
  float    posY;
  float    posX;
  HyFrame* left;
  HyFrame* right;
};

global_variable HyFrame g_rootFrame;

internal void hy_draw_ui_layout(HyRenderer2D* r, HyFrame* frame)
{
  if (!frame)
    return;

  DrawQuad2C(r, (vec2){frame->posX, frame->posY}, (vec2){frame->width, frame->height}, frame->color);

  if (frame->left != NULL)
    hy_draw_ui_layout(r, frame->left);
  if (frame->right != NULL)
    hy_draw_ui_layout(r, frame->right);
}

internal void hy_update_ui_layout(HyFrame* frame, float width, float height, float x, float y)
{
  frame->width = width;
  frame->height = height;
  frame->posX = x;
  frame->posY = y;

  if (!frame->left && !frame->right) {
    return;
  }

  if (frame->left && !frame->right) {
    hy_update_ui_layout(frame->left, width, height, x, y);
    return;
  }

  if (!frame->left && frame->right) {
    hy_update_ui_layout(frame->right, width, height, x, y);
    return;
  }

  float remainingWidth = 0.0f;
  if (frame->left->fixedWidth) {
    remainingWidth = width - frame->left->width;

    hy_update_ui_layout(frame->left, frame->left->width, height, x, y);
    hy_update_ui_layout(frame->right, remainingWidth, height, x + frame->left->width, y);
  } else if (frame->right->fixedWidth) {
    remainingWidth = width - frame->right->width;

    hy_update_ui_layout(frame->left, remainingWidth, height, x, y);
    hy_update_ui_layout(frame->right, frame->right->width, height, x + frame->left->width, y);
  } else {
    hy_update_ui_layout(frame->left, width / 2.0f, height, x, y);
    hy_update_ui_layout(frame->right, width / 2.0f, height, x + width / 2.0f, y);
  }
}

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
    config->user = strdup(value);
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
  hy_update_ui_layout(&g_rootFrame, (float)width, (float)height, 0, 0);
}

// Subsystem:console
int main(int argc, char* argv[])
{
  HY_LogInit();
  HY_Timer_Init();

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

  HyWindow window = {0};
  HY_CreateWindow(&window, config.startMode, "Hyped");
  HY_SetWindowSizeCallback(&window, SizeCallback);

  if (!&window) {
    MessageBox(NULL, "Failed to create window.", "Hyper", MB_ICONERROR);
    ExitProcess(0);
  }

  camera2D = HyCamera2D_Create(1920, 1080, -1, 1);

  HyRenderer2D renderer = {0};
  HyRenderer2D_Init(&renderer);

  HyTexture testTexture = {0};
  HyTexture testTexture1 = {0};
  HyTexture asciiTexture = {0};
  HyTexture_Create(&testTexture, "assets/textures/container.png", HyTextureFilterMode_Linear);
  HyTexture_Create(&testTexture1, "assets/textures/container_specular.png", HyTextureFilterMode_Linear);
  // HyTexture_Create(&asciiTexture, "assets/textures/Fira Code.png", HyTextureFilterMode_Linear);
  HyTexture_Create(&asciiTexture, "assets/textures/DejaVu Sans Mono.png", HyTextureFilterMode_Linear);

  renderer.asciiTexture = &asciiTexture;

  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  float lastTime = 0.0f;

  while (!HY_WindowShouldClose(&window)) {
    float currTime = HY_Timer_GetMilliseconds();
    float dt = currTime - lastTime;

    HyRenderer2DStats stats = HyRenderer2D_GetStats(&renderer);

    HyRenderer2D_ResetStats(&renderer);
    HyRenderer2D_BeginScene(&renderer, &camera2D);
    {
      vec2 pointer = {border + pad, 1000.0f - pad - border - ch};

      HY_SetClearColorCmd(&(HyColor){0.0f, 0.0f, 0.0f, 1.0f});
      HY_ClearCmd();

      static int   count = 0;
      static float cpuLoad = 0.0f;
      static float currCpuLoad = 0.0f;

      cpuLoad = (float)GetCPULoad();
      currCpuLoad += (cpuLoad - currCpuLoad) * (dt / 1000.0f);

      DrawQuad3TC(&renderer, (vec3){500.0f, 300.0f, 0.0f}, (vec2){700.0f, 700.0f}, &testTexture, white);
      DrawQuad3TC(&renderer, (vec3){800.0f, 200.0f, 0.0f}, (vec2){700.0f, 700.0f}, &testTexture1, red0);

      for (uint32_t y = 0; y < 2000; ++y) {
        for (uint32_t x = 0; x < 100; ++x) {
          DrawQuad3TC(&renderer, (vec3){500 + x * 30.0f, y * 30.0f, 0.0f}, (vec2){25.0f, 25.0f}, &testTexture, green1);
        }
      }

      DrawQuad2C(&renderer, (vec3){0.0f, 0.0f}, (vec2){400.0f, 1000.0f}, bg1);
      DrawQuad2C(&renderer, (vec3){border, border}, (vec2){400.0f - border * 2.0f, 1000.0f - border * 2.0f}, bg0);

      DrawQuad2C(&renderer, pointer, (vec2){250.0f, ch}, green0);
      DrawQuad2C(&renderer, pointer, (vec2){250.0f / 32.0f * dt, ch}, green1);
      pointer[1] -= (pad + ch);

      DrawQuad2C(&renderer, pointer, (vec2){250.0f, ch}, blue0);
      DrawQuad2C(&renderer, pointer, (vec2){250.0f * currCpuLoad, ch}, blue1);
      pointer[1] -= (pad + ch);

      DrawQuad2C(&renderer, pointer, (vec2){250.0f, ch}, bg1);
      DrawQuad2C(&renderer, (vec2){pointer[0] + border, pointer[1] + border},
                 (vec2){250.0f - border * 2.0f, ch - border * 2.0f}, bg0);
      pointer[1] -= (pad + ch);

      count++;

      DrawQuad2C(&renderer, (vec2){0.0f, (float)window.height - 300.0f}, (vec2){600.0f, 300.0f}, bg1);

      char glInfo[256] = {0};
      snprintf(glInfo, 256, "Vendor         : %s\nRenderer       : %s\nOpenGL version : %s\nGLSL version   : %s",
               window.glVendor, window.glRenderer, window.glVersion, window.glGLSL);
      draw_debug_text(&renderer, glInfo, 12.0f, window.height - 28.0f, white);

      char drawInfo[256] = {0};
      snprintf(drawInfo, 256, "Renderer Draws: %d Quads: %d", stats.drawCount, stats.quadCount);
      draw_debug_text(&renderer, drawInfo, 12.0f, window.height - 208.0f, white);

      char cpuInfo[256] = {0};
      snprintf(cpuInfo, 256, "CPU Load: %f\nFrame Time: %f", currCpuLoad, dt);
      draw_debug_text(&renderer, cpuInfo, 12.0f, window.height - 258.0f, white);
    }
    HyRenderer2D_EndScene(&renderer);

    HY_SwapBuffers(&window);

    HY_PollEvents(&window);

    lastTime = currTime;

    Sleep(1);
  }

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