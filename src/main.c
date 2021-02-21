#include <windows.h>

#include "resources.h"

#include "hyper_types.h"

static int g_HyperEngineInitialized = true;
static int g_HyperLastErrorCode = 0;

#include "hyper_window.c"

// TODO(alex): What is the right way to add an icon without Visual Studio?

void __stdcall WinMainCRTStartup()
{
    HyWindow window = {0};
    hy_create_window(&window, "Hyped");
    
    if (!&window)
    {
        MessageBox(NULL, "Failed to create window.", "Hyper Error", MB_ICONERROR);
        ExitProcess(0);
    }
    
    while (!HyperWindowShouldClose(&window))
    {
        HyperProcessPendingMessages(&window);
        Sleep(1);
    }
    
    ExitProcess(0);
}
