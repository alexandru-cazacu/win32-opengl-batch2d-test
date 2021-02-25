#pragma warning(disable:4204) // nonstandard extension used : non-constant aggregate initializer

/// @brief Represents a platform-indipendent window.
typedef struct
{
    int borderless;        // is the window currently borderless
    int borderless_resize; // should the window allow resizing by dragging the borders while borderless
    int borderless_drag;   // should the window allow moving my dragging the client area
    int borderless_shadow; // should the window display a native aero shadow while borderless
    b32 shouldClose;
    HWND handle;
    HDC deviceContext;
    HGLRC renderingContext;
    WNDCLASSA windowClass;
    b32 fullscreen;
} HyWindow;

typedef enum HyWindowStartMode
{
    HyWindowStartMode_Auto,
    HyWindowStartMode_Maximized,
    HyWindowStartMode_Fullscreen
} HyWindowStartMode;

// we cannot just use WS_POPUP style
// WS_THICKFRAME: without this the window cannot be resized and so aero snap, de-maximizing and minimizing won't work
// WS_SYSMENU: enables the context menu with the move, close, maximize, minize... commands (shift + right-click on the task bar item)
// WS_CAPTION: enables aero minimize animation/transition
// WS_MAXIMIZEBOX, WS_MINIMIZEBOX: enable minimize/maximize
typedef enum Style {
    StyleWindowed        = WS_OVERLAPPEDWINDOW | WS_THICKFRAME | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_MAXIMIZEBOX,
    StyleAeroBorderless  = WS_POPUP            | WS_THICKFRAME | WS_CAPTION | WS_SYSMENU | WS_MAXIMIZEBOX | WS_MINIMIZEBOX,
    StyleBasicBorderless = WS_POPUP            | WS_THICKFRAME              | WS_SYSMENU | WS_MAXIMIZEBOX | WS_MINIMIZEBOX
} Style;

///
///
///
internal int Win32IsMaximized(HWND hwnd)
{
    WINDOWPLACEMENT placement;
    if (!GetWindowPlacement(hwnd, &placement)) {
        return false;
    }
    
    return placement.showCmd == SW_MAXIMIZE;
}

/* Adjust client rect to not spill over monitor edges when maximized.
 * rect(in/out): in: proposed window rect, out: calculated client rect
 * Does nothing if the window is not maximized.
 */
internal void Win32AdjustMaximizedClientRect(HWND window, RECT* rect)
{
    if (!Win32IsMaximized(window)) {
        return;
    }
    
    HMONITOR monitor = MonitorFromWindow(window, MONITOR_DEFAULTTONULL);
    if (!monitor) {
        return;
    }
    
    MONITORINFO monitorInfo = {0};
    monitorInfo.cbSize = sizeof(monitorInfo);
    if (!GetMonitorInfoA(monitor, &monitorInfo)) {
        return;
    }
    
    // when maximized, make the client area fill just the monitor (without task bar) rect,
    // not the whole window rect which extends beyond the monitor.
    *rect = monitorInfo.rcWork;
    // TODO(alex): Check if works
}

internal int Win32IsCompositionEnabled()
{
    BOOL compositionEnabled = FALSE;
    int success = DwmIsCompositionEnabled(&compositionEnabled) == S_OK;
    return compositionEnabled && success;
}

internal Style Win32SelectBorderlessStyle()
{
    return Win32IsCompositionEnabled() ? StyleAeroBorderless : StyleBasicBorderless;
}

internal void Win32SetShadow(HWND handle, int enabled)
{
    if (Win32IsCompositionEnabled())
    {
        local_persist const MARGINS shadowState[2] = {
            { 0,0,0,0 }, { 1,1,1,1 }
            //{ 0,0,0,0 }
        };
        DwmExtendFrameIntoClientArea(handle, &shadowState[enabled]);
    }
}

internal void Win32SetBorderless(HyWindow* hyWindow, int enabled)
{
    Style newStyle = (enabled) ? Win32SelectBorderlessStyle() : StyleWindowed;
    Style oldStyle = (Style)GetWindowLongPtrA(hyWindow->handle, GWL_STYLE);
    
    if (newStyle != oldStyle)
    {
        hyWindow->borderless = enabled;
        
        SetWindowLongPtrA(hyWindow->handle, GWL_STYLE, (LONG)newStyle);
        
        // when switching between borderless and windowed, restore appropriate shadow state
        Win32SetShadow(hyWindow->handle, hyWindow->borderless_shadow && (newStyle != StyleWindowed));
        
        // redraw frame
        SetWindowPos(hyWindow->handle, NULL, 0, 0, 0, 0, SWP_FRAMECHANGED | SWP_NOMOVE | SWP_NOSIZE);
        ShowWindow(hyWindow->handle, SW_SHOW);
    }
}

internal void Win32SetBorderlessShadow(HyWindow* hyWindow, int enabled)
{
    if (hyWindow->borderless)
    {
        hyWindow->borderless_shadow = enabled;
        Win32SetShadow(hyWindow->handle, enabled);
    }
}

internal LRESULT Win32HitTest(HyWindow* hyWindow, POINT cursor)
{
    // identify borders and corners to allow resizing the window.
    // Note: On Windows 10, windows behave differently and
    // allow resizing outside the visible window frame.
    // This implementation does not replicate that behavior.
    const POINT border = {
        GetSystemMetrics(SM_CXFRAME) + GetSystemMetrics(SM_CXPADDEDBORDER),
        GetSystemMetrics(SM_CYFRAME) + GetSystemMetrics(SM_CXPADDEDBORDER)
    };
    RECT window;
    if (!GetWindowRect(hyWindow->handle, &window))
    {
        return HTNOWHERE;
    }
    
    const int drag = hyWindow->borderless_drag ? HTCAPTION : HTCLIENT;
    
    enum region_mask {
        client = 0b0000,
        left   = 0b0001,
        right  = 0b0010,
        top    = 0b0100,
        bottom = 0b1000,
    };
    
    const int result =
        left    * (cursor.x <  (window.left   + border.x)) |
        right   * (cursor.x >= (window.right  - border.x)) |
        top     * (cursor.y <  (window.top    + border.y)) |
        bottom  * (cursor.y >= (window.bottom - border.y));
    
    switch (result)
    {
        case left          : return hyWindow->borderless_resize ? HTLEFT        : drag;
        case right         : return hyWindow->borderless_resize ? HTRIGHT       : drag;
        case top           : return hyWindow->borderless_resize ? HTTOP         : drag;
        case bottom        : return hyWindow->borderless_resize ? HTBOTTOM      : drag;
        case top | left    : return hyWindow->borderless_resize ? HTTOPLEFT     : drag;
        case top | right   : return hyWindow->borderless_resize ? HTTOPRIGHT    : drag;
        case bottom | left : return hyWindow->borderless_resize ? HTBOTTOMLEFT  : drag;
        case bottom | right: return hyWindow->borderless_resize ? HTBOTTOMRIGHT : drag;
        case client        : return drag;
        default            : return HTNOWHERE;
    }
}

internal LRESULT CALLBACK Win32WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    // NOTE(alex): Only on windows. Probably we are still bootrsapping the OGL context so we skip message handling to avoid quitting the window prematurely (if for exemple we destroy the trampoline context).
    if (g_HyperRendererBootstrapping) {
        return DefWindowProcA(hwnd, msg, wParam, lParam);;
    }
    
    if (msg == WM_NCCREATE)
    {
        LPVOID userdata = ((CREATESTRUCTW*)lParam)->lpCreateParams;
        // store window instance pointer in window user data
        SetWindowLongPtrA(hwnd, GWLP_USERDATA, (LONG_PTR)userdata);
    }
    
    HyWindow* hyWindow = (HyWindow*)(GetWindowLongPtrA(hwnd, GWLP_USERDATA));
    
    if (hyWindow) {
        switch(msg) {
#if 0
            case WM_ACTIVATEAPP: {
                HDC deviceContext = GetDC(window);
                Vec2 dimension =  GetWindowDimension();
                Win32InitOpenGL(window);
                Win32DisplayBufferInWindow(deviceContext, dimension.Width, dimension.Height);
            } break;
#endif
            
            case WM_NCCALCSIZE: {
                if (wParam == TRUE && hyWindow->borderless) {
                    NCCALCSIZE_PARAMS params = *(NCCALCSIZE_PARAMS*)lParam;
                    Win32AdjustMaximizedClientRect(hwnd, &params.rgrc[0]);
                    return 0;
                }
            } break;
            
            case WM_NCHITTEST: {
                // When we have no border or title bar, we need to perform our
                // own hit testing to allow resizing and moving.
                if (hyWindow->borderless) {
                    POINT p = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
                    return Win32HitTest(hyWindow, p);
                }
            } break;
            
            case WM_NCACTIVATE: {
                if (!Win32IsCompositionEnabled()) {
                    // Prevents window frame reappearing on window activation
                    // in "basic" theme, where no aero shadow is present.
                    return 1;
                }
            } break;
            
            // TODO(Alex): Possibily ask for confirmation 
            case WM_CLOSE: { // X pressed
                DestroyWindow(hwnd);
            } break;
            
            case WM_DESTROY: { // After the window is removed, before all child windows are destroyed
                PostQuitMessage(0);
            } break;
            
#if 0
            case WM_SYSKEYDOWN:
            case WM_SYSKEYUP:
            case WM_KEYDOWN:
            case WM_KEYUP: {
                //Assert(!"Keyboard input came in through a non-dispatch message");
            } break;
#endif
            
#if 0
            case WM_SIZE: {
                //Win32WindowDimension dimension =  GetWindowDimension(window);
                //Win32ResizeDIBSection(&g_Backbuffer, dimension.Width, dimension.Height);
            } break;
#endif
        }
    }
    
    return DefWindowProcA(hwnd, msg, wParam, lParam);
}

//~
// Public facing functions.
//

internal void HySwapBuffers(HyWindow* window)
{
    SwapBuffers(window->deviceContext);
}

///
///
///
internal b32 HY_WindowShouldClose(HyWindow* window)
{
    return window->shouldClose;
}

///
///
///
internal void HY_PollEvents(HyWindow* window)
{
    MSG message;
    
    // NOTE(alex): GetMessage = blocking, PeekMessage = non blocking
    while (PeekMessageA(&message, 0, 0, 0, PM_REMOVE)) {
        switch(message.message) {
            case WM_QUIT: { // received from PostQuitMessage()
                window->shouldClose = true;
            } break;
            
            case WM_KEYDOWN: {
                u32 vkCode = (u32)message.wParam;
                if (vkCode == 0x1B) {
                    window->shouldClose = true;
                }
            } break;
            
            default: {
                TranslateMessage(&message);
                DispatchMessage(&message); // Dispatch to Win32WindowCallback()
            }
        }
    }
}

///
/// Creates a new Window with a modern OpenGL context.
///
internal int HY_CreateWindow(HyWindow* hyWindow, HyWindowStartMode startMode, const char* title)
{
    if (!g_HyperEngineInitialized) {
        return HY_NOT_INITIALIZED;
    }
    
    HINSTANCE hInstance = GetModuleHandle(NULL);
    
    WNDCLASSA window_class = {0};
    window_class.style = CS_OWNDC | CS_HREDRAW | CS_VREDRAW;
    window_class.lpfnWndProc = Win32WndProc;
    window_class.hInstance = hInstance;
    window_class.hCursor = LoadCursor(NULL, IDC_ARROW);
    window_class.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(101));
    //window_class.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
    window_class.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    window_class.lpszClassName = "HyperWindowClass";
    
    RegisterClassA(&window_class);
    
    // NOTE(alex): It's dangerous to go alone. Take this: https://mariuszbartosik.com/opengl-4-x-initialization-in-windows-without-a-framework/
    
    g_HyperRendererBootstrapping = true;
    
    HWND fakeWND = CreateWindow("HyperWindowClass", // window class
                                "Fake Window",      // title
                                (DWORD)StyleAeroBorderless, // style
                                CW_USEDEFAULT, CW_USEDEFAULT, // x, y
                                CW_USEDEFAULT, CW_USEDEFAULT, // width, height
                                NULL, NULL,                 // parent window, menu
                                hInstance, hyWindow);       // instance, param
    
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
    
    g_HyperRendererBootstrapping = false;
    
    HWND WND = CreateWindowA(window_class.lpszClassName, title, // class name, window name
                             (DWORD)StyleAeroBorderless, // style
                             CW_USEDEFAULT, CW_USEDEFAULT, // x, y
                             1280, 720, // width, height
                             0, 0, hInstance, hyWindow); // parent window, menu, instance. param
    
    hyWindow->handle = WND;
    
    HDC DC = GetDC(WND);
    
    g_HyperRendererBootstrapping = true;
    
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
    
    g_HyperRendererBootstrapping = false;
    
    wglMakeCurrent(NULL, NULL);
    wglDeleteContext(fakeRC);
    ReleaseDC(fakeWND, fakeDC);
    DestroyWindow(fakeWND);
    if (!wglMakeCurrent(DC, RC)) {
        MessageBox(NULL, "wglMakeCurrent() failed.", "Hyper Error", MB_ICONERROR);
        return HY_PLATFORM_ERROR;
    }
    
    g_HyperRendererBootstrapping = false;
    
    hyWindow->handle = WND;
    hyWindow->deviceContext = DC;
    
    hyWindow->borderless = true;
    hyWindow->borderless_resize = true;
    hyWindow->borderless_drag = true;
    hyWindow->borderless_shadow = true;
    
    Win32SetBorderless(hyWindow, true);
    Win32SetBorderlessShadow(hyWindow, true);
    SetWindowTextA(WND, (LPCSTR)glGetString(GL_VERSION));
    ShowWindow(hyWindow->handle, SW_SHOW);
    
    return HY_NO_ERROR;
}