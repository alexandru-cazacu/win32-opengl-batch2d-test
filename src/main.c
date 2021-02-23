#include "win32_lean_and_mean.c"
#include "win32_crt.c"

#include <gl/gl.h>
#include "gl/glext.h"
#include "gl/wglext.h"
#include "resources.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#define STB_IMAGE_RESIZE_IMPLEMENTATION
#include <stb_image_resize.h>

#include "hy_log.c"
#include "hyper_types.h"
#include "hyper_window.c"
#include "hy_renderer_2d.c"
#include "hy_time.c"

// TODO(alex): What is the right way to add an icon without Visual Studio?

//void __stdcall WinMainCRTStartup()
int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow)
{
    HyWindow window = {0};
    HyCreateWindow(&window, "Hyped");
    
    HY_TRACE("Initializing...");
    
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
