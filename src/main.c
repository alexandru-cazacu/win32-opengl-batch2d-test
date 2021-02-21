#include <windows.h>
#include <gl/gl.h>
#include "gl/glext.h"
#include "gl/wglext.h"

#include "resources.h"

#include "win32_crt.c"
#include "hyper_types.h"

static int g_HyperEngineInitialized = true;
static int g_HyperLastErrorCode = 0;
static int g_HyperRendererInitialized = false;

#include "hyper_window.c"

// TODO(alex): What is the right way to add an icon without Visual Studio?

void __stdcall WinMainCRTStartup()
{
    HyWindow window = {0};
    HyCreateWindow(&window, "Hyped");
    
    if (!&window) {
        MessageBox(NULL, "Failed to create window.", "Hyper Error", MB_ICONERROR);
        ExitProcess(0);
    }
    
    while (!HyWindowShouldClose(&window)) {
        HyProcessPendingMessages(&window);
        
        glClearColor(0.129f, 0.586f, 0.949f, 1.0f); // rgb(33,150,243)
        glClear(GL_COLOR_BUFFER_BIT);
        
        HySwapBuffers(&window);
        Sleep(1);
    }
    
    ExitProcess(0);
}
