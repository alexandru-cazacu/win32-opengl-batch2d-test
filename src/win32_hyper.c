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

#include <windows.h>
#include <windowsx.h>
#include <dwmapi.h>
#include <shellapi.h>
#include <gl/gl.h>
#include <gl/glext.h>
#include <gl/wglext.h>

#pragma warning(disable:4204) // nonstandard extension used : non-constant aggregate initializer
#pragma warning(disable:4996) // // TODO(alex): What error
#include <cglm/cglm.h>
#include <cglm/cam.h>

#pragma warning(disable:4459) // TODO(alex): What error
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#define STB_IMAGE_RESIZE_IMPLEMENTATION
//#include <stb_image_resize.h>

#include "hy_types.c"
#include "hy_log.c"
#include "resources.h"
#include "hy_file.c"
#include "hy_time.c"
#include "win32_renderer_opengl.c"
#include "win32_window.c"

#pragma warning(disable:4996)
#include "ini.h"
#include "ini.c"

// TODO(alex): What is the right way to add an icon without Visual Studio?

typedef struct
{
    HyWindowStartMode startMode;
    const char* user;
} HyConfig;

HyCamera2D camera2D;

internal int configHandler(void* user, const char* section, const char* name, const char* value)
{
    HyConfig* config = (HyConfig*)user;
    
#define MATCH(s, n) strcmp(section, s) == 0 && strcmp(name, n) == 0
    if (MATCH("window", "startMode")) {
        //config->startMode = strdup(value);
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
        return 0;  /* unknown section/name, error */
    }
    return 1;
}

internal void SizeCallback(HyWindow* hyWindow, unsigned int width, unsigned int height)
{
    printf("Resize callback (%d, %d)\n", width, height);
    //glViewport(0, 0, width, height);
    //glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    //glClear(GL_COLOR_BUFFER_BIT);
    //HY_SwapBuffers(hyWindow);
    
    HyCamera2D_Resize(&camera2D, (float)width, (float)height, -100.0f, 0.0f);
}

// Subsystem:console
int main(int argc, char *argv[])
{
    HY_LogInit(false);
    
    HyConfig config = {0};
    config.startMode = HyWindowStartMode_Auto;
    
    HyFile* configFile = HY_ReadFile(".hypedrc");
    if (configFile) {
        ini_parse_string(configFile->data, configHandler, &config);
        fprintf(stdout,
                "Config loaded from '.hypedrc': \n - startMode=%d\n - user=%s\n",
                config.startMode,
                config.user);
    } else {
        HY_ERROR(".hypedrc not found");
    }
    
    HyWindow window = {0};
    HY_SetWindowSizeCallback(&window, SizeCallback);
    HY_CreateWindow(&window, config.startMode, "Hyped");
    
    if (!&window) {
        MessageBox(NULL, "Failed to create window.", "Hyper Error", MB_ICONERROR);
        ExitProcess(0);
    }
    
    camera2D = HyCamera2D_Create(1920, 1080, 0, 1);
    HyShader* simpleVert = HY_Shader_Create("assets/shaders/simple.vert", "assets/shaders/simple.frag");
    
    HyRenderer2D renderer = {0};
    HyRenderer2D_Init(&renderer);
    
#if 0
    // set up vertex data (and buffer(s)) and configure vertex attributes
    // ------------------------------------------------------------------
    float vertices[] = {
        -1.0f, 0.85f, 0.0f, // left  
        1.0f, 1.0f, 0.0f, // right 
        -1.0f,  1.0f, 0.0f  // top   
    }; 
    
    unsigned int VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
    glBindVertexArray(VAO);
    
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    
    // note that this is allowed, the call to glVertexAttribPointer registered VBO as the vertex attribute's bound vertex buffer object so afterwards we can safely unbind
    glBindBuffer(GL_ARRAY_BUFFER, 0); 
    
    // You can unbind the VAO afterwards so other VAO calls won't accidentally modify this VAO, but this rarely happens. Modifying other
    // VAOs requires a call to glBindVertexArray anyways so we generally don't unbind VAOs (nor VBOs) when it's not directly necessary.
    glBindVertexArray(0); 
#endif
    
    while (!HY_WindowShouldClose(&window)) {
        //glClearColor(0.129f, 0.586f, 0.949f, 1.0f); // rgb(33,150,243)
        //glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // rgb(33,150,243)
        //glClearColor(0.1f, 0.1f, 0.1f, 1.0f); // rgb(33,150,243)
        //glClear(GL_COLOR_BUFFER_BIT);
        
        HY_SetClearColorCmd(&(HyColor){0.1f, 0.1f, 0.1f, 0.1f});
        HY_ClearCmd();
        
        
        //HY_Shader_Bind(simpleVert);
        //glUseProgram(shaderProgram);
        //glBindVertexArray(VAO); // seeing as we only have a single VAO there's no need to bind it every time, but we'll do so to keep things a bit more organized
        //glDrawArrays(GL_TRIANGLES, 0, 3);
        
        //DrawQuad(&renderer, (vec3){0.0f, 0.0f, 0.0f}, (vec2){1.0f, 1.0f}, 
        
        HyRenderer2D_ResetStats(&renderer);
        HyRenderer2D_BeginScene(&renderer, &camera2D);
        DrawQuad3C(&renderer, (vec3){0.0f, 0.0f, 0.0f}, (vec2){1920.0f, 100.0f}, (vec4){1.0f, 0.0f, 1.0f, 1.0f});
        HyRenderer2D_EndScene(&renderer);
        
        HY_SwapBuffers(&window);
        
        HY_PollEvents(&window);
        
        Sleep(1);
    }
    
#if 0
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    HY_Shader_Delete(simpleVert);
#endif
    
    ExitProcess(0);
}

// Subsystem:windows
int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow)
{
    // Read cmd args.
    LPWSTR* argv;
    int argc;
    int i;
    
    //argv = CommandLineToArgvW(GetCommandLineW(), &argc);
    argv = CommandLineToArgvW(pCmdLine, &argc);
    if( NULL == argv ) {
        printf("CommandLineToArgvW failed\n");
        return 0;
    }
    else {
        for( i=0; i<argc; i++) {
            printf("%d: %ws\n", i, argv[i]);
        }
    }
    
    // TODO(alex): How tf do u use windows types?
    int result = main(argc, NULL);
    
    LocalFree(argv); // Free memory allocated for CommandLineToArgvW arguments.
    
    return result;
}