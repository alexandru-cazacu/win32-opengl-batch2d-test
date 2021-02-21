#pragma warning(disable:4204) // nonstandard extension used : non-constant aggregate initializer

/// @brief Represents a platform-indipendent window.
typedef struct
{
    HWND WindowHandle;
    b32 ShouldClose;
    HDC DeviceContext;
} HyWindow;

internal b32 HyWindowShouldClose(HyWindow* window)
{
    return window->ShouldClose;
}

internal void HyProcessPendingMessages(HyWindow* window)
{
    MSG message;
    
    // NOTE(alex): GetMessage = blocking, PeekMessage = non blocking
    while (PeekMessageA(&message, 0, 0, 0, PM_REMOVE)) {
        switch(message.message) {
            case WM_QUIT: { // received from PostQuitMessage()
                window->ShouldClose = true;
            } break;
            
            case WM_KEYDOWN: {
                u32 vkCode = (u32)message.wParam;
                if (vkCode == 0x1B) {
                    window->ShouldClose = true;
                }
            } break;
            
            default: {
                TranslateMessage(&message);
                DispatchMessage(&message); // Dispatch to Win32WindowCallback()
            }
        }
    }
}

internal LRESULT CALLBACK Win32WindowCallback(HWND window, UINT message, WPARAM wParam, LPARAM lParam)
{
    // NOTE(alex): Only on windows. Probably we are still bootrsapping the OGL context so we skip message handling to avoid quitting the window prematurely (if for exemple we destroy the trampoline context).
    if (!g_HyperRendererInitialized) {
        return DefWindowProcA(window, message, wParam, lParam);;
    }
    
    LRESULT result = 0;
    
    switch(message) {
        case WM_ACTIVATEAPP: {
#if 0
            HDC deviceContext = GetDC(window);
            Vec2 dimension =  GetWindowDimension();
            Win32InitOpenGL(window);
            Win32DisplayBufferInWindow(deviceContext, dimension.Width, dimension.Height);
#endif
        } break;
        
        // TODO(Alex): Possibily ask for confirmation 
        case WM_CLOSE: { // X pressed
            DestroyWindow(window);
        } break;
        
        case WM_DESTROY: { // After the window is removed, before all child windows are destroyed
            PostQuitMessage(0);
        } break;
        
        case WM_SYSKEYDOWN:
        case WM_SYSKEYUP:
        case WM_KEYDOWN:
        case WM_KEYUP: {
            Assert(!"Keyboard input came in through a non-dispatch message");
        } break;
        
        case WM_SIZE: {
            //Win32WindowDimension dimension =  GetWindowDimension(window);
            //Win32ResizeDIBSection(&g_Backbuffer, dimension.Width, dimension.Height);
        } break;
        
        default: {
            result = DefWindowProcA(window, message, wParam, lParam);
        } break;
    }
    
    return result;
}

internal void HySwapBuffers(HyWindow* window)
{
    SwapBuffers(window->DeviceContext);
}

/// @brief Creates a new Window with a modern OpenGL context.
internal int HyCreateWindow(HyWindow* window, const char* title)
{
    if (!g_HyperEngineInitialized) {
        return HY_NOT_INITIALIZED;
    }
    
    WNDCLASSA window_class = {0};
    HINSTANCE hInstance = GetModuleHandle(NULL);
    
    window_class.style = CS_OWNDC | CS_HREDRAW | CS_VREDRAW;
    window_class.lpfnWndProc = Win32WindowCallback;
    window_class.hInstance = hInstance;
    window_class.hCursor = LoadCursor(NULL, IDC_ARROW);
    window_class.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(101));
    window_class.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
    window_class.lpszClassName = "HyperWindowClass";
    
    RegisterClassA(&window_class);
    
    // NOTE(alex): It's dangerous to go alone. Take this: https://mariuszbartosik.com/opengl-4-x-initialization-in-windows-without-a-framework/
    
    HWND fakeWND = CreateWindow("HyperWindowClass", // window class
                                "Fake Window",      // title
                                WS_CLIPSIBLINGS | WS_CLIPCHILDREN, // style
                                0, 0,                       // position x, y
                                1, 1,                       // width, height
                                NULL, NULL,                 // parent window, menu
                                hInstance, NULL);           // instance, param
    
    HDC fakeDC = GetDC(fakeWND);        // Device Context
    
    PIXELFORMATDESCRIPTOR fakePFD;
    ZeroMemory(&fakePFD, sizeof(fakePFD));
    fakePFD.nSize = sizeof(fakePFD);
    fakePFD.nVersion = 1;
    fakePFD.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
    fakePFD.iPixelType = PFD_TYPE_RGBA;
    fakePFD.cColorBits = 32;
    fakePFD.cAlphaBits = 8;
    fakePFD.cDepthBits = 24;
    
    int fakePFDID = ChoosePixelFormat(fakeDC, &fakePFD);
    if (fakePFDID == 0) {
        MessageBox(NULL, "ChoosePixelFormat() failed.", "Hyper Error", MB_ICONERROR);
        return HY_PLATFORM_ERROR;
    }
    
    if (SetPixelFormat(fakeDC, fakePFDID, &fakePFD) == false) {
        MessageBox(NULL, "SetPixelFormat() failed.", "Hyper Error", MB_ICONERROR);
        return HY_PLATFORM_ERROR;
    }
    
    HGLRC fakeRC = wglCreateContext(fakeDC);    // Rendering Contex
    
    if (fakeRC == 0) {
        MessageBox(NULL, "wglCreateContext() failed.", "Hyper Error", MB_ICONERROR);
        return HY_PLATFORM_ERROR;
    }
    
    if (wglMakeCurrent(fakeDC, fakeRC) == false) {
        MessageBox(NULL, "wglMakeCurrent() failed.", "Hyper Error", MB_ICONERROR);
        return HY_PLATFORM_ERROR;
    }
    
    PFNWGLCHOOSEPIXELFORMATARBPROC wglChoosePixelFormatARB = NULL;
    wglChoosePixelFormatARB = (PFNWGLCHOOSEPIXELFORMATARBPROC)(wglGetProcAddress("wglChoosePixelFormatARB"));
    if (wglChoosePixelFormatARB == NULL) {
        MessageBox(NULL, "wglGetProcAddress() failed.", "Hyper Error", MB_ICONERROR);
        return HY_PLATFORM_ERROR;
    }
    
    PFNWGLCREATECONTEXTATTRIBSARBPROC wglCreateContextAttribsARB = NULL;
    wglCreateContextAttribsARB = (PFNWGLCREATECONTEXTATTRIBSARBPROC)(wglGetProcAddress("wglCreateContextAttribsARB"));
    if (wglCreateContextAttribsARB == NULL) {
        MessageBox(NULL, "wglGetProcAddress() failed.", "Hyper Error", MB_ICONERROR);
        return HY_PLATFORM_ERROR;
    }
    
    HWND WND = CreateWindowA(window_class.lpszClassName, title, // class name, window name
                             WS_VISIBLE | WS_OVERLAPPEDWINDOW, // style
                             CW_USEDEFAULT, CW_USEDEFAULT, // x, y
                             CW_USEDEFAULT, CW_USEDEFAULT, // width, height
                             0, 0, hInstance, 0); // parent window, menu, instance. param
    
    HDC DC = GetDC(WND);
    
    const int pixelAttribs[] = {
        WGL_DRAW_TO_WINDOW_ARB, GL_TRUE,
        WGL_SUPPORT_OPENGL_ARB, GL_TRUE,
        WGL_DOUBLE_BUFFER_ARB, GL_TRUE,
        WGL_PIXEL_TYPE_ARB, WGL_TYPE_RGBA_ARB,
        WGL_ACCELERATION_ARB, WGL_FULL_ACCELERATION_ARB,
        WGL_COLOR_BITS_ARB, 32,
        WGL_ALPHA_BITS_ARB, 8,
        WGL_DEPTH_BITS_ARB, 24,
        WGL_STENCIL_BITS_ARB, 8,
        WGL_SAMPLE_BUFFERS_ARB, GL_TRUE,
        WGL_SAMPLES_ARB, 4,
        0
    };
    
    int pixelFormatID; UINT numFormats;
    BOOL status = wglChoosePixelFormatARB(DC, pixelAttribs, NULL, 1, &pixelFormatID, &numFormats);
    
    if (status == false || numFormats == 0) {
        MessageBox(NULL, "wglChoosePixelFormatARB() failed.", "Hyper Error", MB_ICONERROR);
        return HY_PLATFORM_ERROR;
    }
    
    PIXELFORMATDESCRIPTOR PFD;
    DescribePixelFormat(DC, pixelFormatID, sizeof(PFD), &PFD);
    SetPixelFormat(DC, pixelFormatID, &PFD);
    
    const int major_min = 4, minor_min = 5;
    int  contextAttribs[] = {
        WGL_CONTEXT_MAJOR_VERSION_ARB, major_min,
        WGL_CONTEXT_MINOR_VERSION_ARB, minor_min,
        WGL_CONTEXT_PROFILE_MASK_ARB, WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
        0
    };
    
    HGLRC RC = wglCreateContextAttribsARB(DC, 0, contextAttribs);
    if (RC == NULL) {
        MessageBox(NULL, "wglCreateContextAttribsARB() failed.", "Hyper Error", MB_ICONERROR);
        return HY_PLATFORM_ERROR;
    }
    
    wglMakeCurrent(NULL, NULL);
    wglDeleteContext(fakeRC);
    ReleaseDC(fakeWND, fakeDC);
    DestroyWindow(fakeWND);
    if (!wglMakeCurrent(DC, RC)) {
        MessageBox(NULL, "wglMakeCurrent() failed.", "Hyper Error", MB_ICONERROR);
        return HY_PLATFORM_ERROR;
    }
    
    g_HyperRendererInitialized = true;
    
    SetWindowTextA(WND, (LPCSTR)glGetString(GL_VERSION));
    ShowWindow(WND, TRUE);
    
    window->WindowHandle = WND;
    window->DeviceContext = DC;
    
    return HY_NO_ERROR;
}