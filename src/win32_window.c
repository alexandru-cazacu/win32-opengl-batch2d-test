//~
/// Metrics
///

static float CalculateCPULoad(unsigned long long idleTicks, unsigned long long totalTicks)
{
    static unsigned long long _previousTotalTicks = 0;
    static unsigned long long _previousIdleTicks = 0;
    
    unsigned long long totalTicksSinceLastTime = totalTicks - _previousTotalTicks;
    unsigned long long idleTicksSinceLastTime = idleTicks - _previousIdleTicks;
    
    float ret = 1.0f - ((totalTicksSinceLastTime > 0) ? ((float)idleTicksSinceLastTime) / totalTicksSinceLastTime : 0);
    
    _previousTotalTicks = totalTicks;
    _previousIdleTicks = idleTicks;
    return ret;
}

internal unsigned long long FileTimeToInt64(FILETIME ft)
{
    return (((unsigned long long)(ft.dwHighDateTime)) << 32) | ((unsigned long long)ft.dwLowDateTime);
}

// Returns 1.0f for "CPU fully pinned", 0.0f for "CPU idle", or somewhere in
// between You'll need to call this at regular intervals, since it measures the
// load between the previous call and the current one.  Returns -1.0 on error.
float GetCPULoad()
{
    FILETIME idleTime, kernelTime, userTime;
    return GetSystemTimes(&idleTime, &kernelTime, &userTime)
        ? CalculateCPULoad(FileTimeToInt64(idleTime), FileTimeToInt64(kernelTime) + FileTimeToInt64(userTime))
        : -1.0f;
}

//~
/// Memory
///

#define hy_malloc(nbytes)        _hy_malloc(nbytes, __FUNCTION__)
#define hy_calloc(count, nbytes) _hy_calloc(count, nbytes, __FUNCTION__)
#define hy_realloc(p, nbytes)    _hy_realloc(p, nbytes, __FUNCTION__)
#define hy_free(p)               _hy_free(p, __FUNCTION__)

// TODO(alex): Replace VirtualAlloc with malloc
// TODO(alex): Create memory allocator that overrides malloc
// TODO(alex): Plot memory usage as a chart.

void* _hy_malloc(size_t nbytes, char* funcName)
{
    HY_INFO("%s \tmalloc(%d)", funcName, nbytes);
    return malloc(nbytes);
}

void* _hy_calloc(size_t count, size_t nbytes)
{
    HY_INFO("\tcalloc");
    return calloc(count, nbytes);
}

void* _hy_realloc(void* p, size_t nbytes)
{
    HY_INFO("\trealloc");
    return realloc(p, nbytes);
}

void _hy_free(void* p, char* funcName)
{
    HY_INFO("%s \tfree()", funcName);
    free(p);
}

//~
/// Window.
///

typedef struct HyWindow HyWindow;

typedef void (*window_size_callback_t)(HyWindow*, unsigned int, unsigned int);

/// @brief Represents a platform-indipendent window.
struct HyWindow {
    int borderless;        // is the window currently borderless
    int borderless_resize; // should the window allow resizing by dragging the
    // borders while borderless
    int borderless_drag;   // should the window allow moving my dragging the client
    // area
    int borderless_shadow; // should the window display a native aero shadow while
    // borderless
    
    b32                    shouldClose;
    HWND                   handle;
    HDC                    deviceContext;
    HGLRC                  renderingContext;
    WNDCLASSA              windowClass;
    WINDOWPLACEMENT        prevPos;
    fglOpenGLContext       glContext;
    window_size_callback_t sizeCallback;
    
    b32 fullscreen;
    int width;
    int height;
    
    const char* glVendor;
    const char* glRenderer;
    const char* glVersion;
    const char* glGLSL;
};

typedef enum HyWindowStartMode {
    HyWindowStartMode_Auto,
    HyWindowStartMode_Maximized,
    HyWindowStartMode_Fullscreen
} HyWindowStartMode;

// we cannot just use WS_POPUP style
// WS_THICKFRAME: without this the window cannot be resized and so aero snap,
// de-maximizing and minimizing won't work WS_SYSMENU: enables the context menu
// with the move, close, maximize, minize... commands (shift + right-click on
// the task bar item) WS_CAPTION: enables aero minimize animation/transition
// WS_MAXIMIZEBOX, WS_MINIMIZEBOX: enable minimize/maximize
typedef enum Style {
    StyleWindowed = WS_OVERLAPPEDWINDOW | WS_THICKFRAME | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_MAXIMIZEBOX,
    StyleAeroBorderless = WS_POPUP | WS_THICKFRAME | WS_CAPTION | WS_SYSMENU | WS_MAXIMIZEBOX | WS_MINIMIZEBOX,
    StyleBasicBorderless = WS_POPUP | WS_THICKFRAME | WS_SYSMENU | WS_MAXIMIZEBOX | WS_MINIMIZEBOX
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
    
    // when maximized, make the client area fill just the monitor (without task
    // bar) rect, not the whole window rect which extends beyond the monitor.
    *rect = monitorInfo.rcWork;
}

internal int Win32IsCompositionEnabled()
{
    BOOL compositionEnabled = FALSE;
    int  success = DwmIsCompositionEnabled(&compositionEnabled) == S_OK;
    return compositionEnabled && success;
}

internal Style Win32SelectBorderlessStyle()
{
    return Win32IsCompositionEnabled() ? StyleAeroBorderless : StyleBasicBorderless;
}

internal void Win32SetShadow(HWND handle, int enabled)
{
    if (Win32IsCompositionEnabled()) {
        local_persist const MARGINS shadowState[2] = {{0, 0, 0, 0}, {1, 1, 1, 1}};
        DwmExtendFrameIntoClientArea(handle, &shadowState[enabled]);
    }
}

internal void Win32SetBorderless(HyWindow* hyWindow, int enabled)
{
    Style newStyle = (enabled) ? Win32SelectBorderlessStyle() : StyleWindowed;
    Style oldStyle = (Style)GetWindowLongPtrA(hyWindow->handle, GWL_STYLE);
    
    if (newStyle != oldStyle) {
        hyWindow->borderless = enabled;
        
        SetWindowLongPtrA(hyWindow->handle, GWL_STYLE, (LONG)newStyle);
        
        // when switching between borderless and windowed, restore appropriate
        // shadow state
        Win32SetShadow(hyWindow->handle, hyWindow->borderless_shadow && (newStyle != StyleWindowed));
        
        // redraw frame
        SetWindowPos(hyWindow->handle, NULL, 0, 0, 0, 0, SWP_FRAMECHANGED | SWP_NOMOVE | SWP_NOSIZE);
        ShowWindow(hyWindow->handle, SW_SHOW);
    }
}

internal void Win32SetBorderlessShadow(HyWindow* hyWindow, int enabled)
{
    if (hyWindow->borderless) {
        hyWindow->borderless_shadow = enabled;
        Win32SetShadow(hyWindow->handle, enabled);
    }
}

internal LRESULT Win32HitTest(HyWindow* hyWindow, POINT cursor)
{
    // Exit early or we will get artifacts where window stays fullscreen but is
    // also draggable.
    if (hyWindow->fullscreen) {
        return HTNOWHERE;
    }
    
    // identify borders and corners to allow resizing the window.
    // Note: On Windows 10, windows behave differently and
    // allow resizing outside the visible window frame.
    // This implementation does not replicate that behavior.
    const POINT border = {GetSystemMetrics(SM_CXFRAME) + GetSystemMetrics(SM_CXPADDEDBORDER),
        GetSystemMetrics(SM_CYFRAME) + GetSystemMetrics(SM_CXPADDEDBORDER)};
    RECT        window;
    if (!GetWindowRect(hyWindow->handle, &window)) {
        return HTNOWHERE;
    }
    
    // const int drag = hyWindow->borderless_drag ? HTCAPTION : HTCLIENT;
    
    enum region_mask {
        client = 0b0000,
        left = 0b0001,
        right = 0b0010,
        top = 0b0100,
        bottom = 0b1000,
    };
    
    int drag;
    if ((cursor.x < window.right) && (cursor.x >= window.left) && (cursor.y < (window.top + 50)) &&
        (cursor.y >= window.top)) {
        drag = HTCAPTION;
    } else {
        drag = HTCLIENT;
    }
    
    const int result = left * (cursor.x < (window.left + border.x)) | right * (cursor.x >= (window.right - border.x)) |
        top * (cursor.y < (window.top + border.y)) | bottom * (cursor.y >= (window.bottom - border.y));
    
    switch (result) {
        case left: return hyWindow->borderless_resize ? HTLEFT : drag;
        case right: return hyWindow->borderless_resize ? HTRIGHT : drag;
        case top: return hyWindow->borderless_resize ? HTTOP : drag;
        case bottom: return hyWindow->borderless_resize ? HTBOTTOM : drag;
        case top | left: return hyWindow->borderless_resize ? HTTOPLEFT : drag;
        case top | right: return hyWindow->borderless_resize ? HTTOPRIGHT : drag;
        case bottom | left: return hyWindow->borderless_resize ? HTBOTTOMLEFT : drag;
        case bottom | right: return hyWindow->borderless_resize ? HTBOTTOMRIGHT : drag;
        case client: return drag;
        default: return HTNOWHERE;
    }
}

internal LRESULT CALLBACK Win32WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    if (msg == WM_NCCREATE) {
        LPVOID userdata = ((CREATESTRUCTW*)lParam)->lpCreateParams;
        // store window instance pointer in window user data
        SetWindowLongPtrA(hwnd, GWLP_USERDATA, (LONG_PTR)userdata);
    }
    
    HyWindow* hyWindow = (HyWindow*)(GetWindowLongPtrA(hwnd, GWLP_USERDATA));
    
    if (hyWindow) {
        switch (msg) {
#if 0
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
            
            case WM_PAINT: {
                COLORREF bkcolor = RGB(25,25,25);
                PAINTSTRUCT ps;
                RECT rc;
                HDC hdc = BeginPaint(hwnd, &ps);
                GetClientRect(hwnd, &rc);
                SetDCBrushColor(hdc, bkcolor);
                FillRect(hdc, &rc, (HBRUSH)GetStockObject(DC_BRUSH));
                EndPaint(hwnd, &ps);
                return 0;
            } break;
            
            case WM_ERASEBKGND : {
                COLORREF bkcolor = RGB(25,25,25);
                PAINTSTRUCT ps;
                HDC hdc = BeginPaint(hwnd, &ps);
                SetDCBrushColor(hdc, bkcolor);
                FillRect(hdc, &ps.rcPaint, CreateSolidBrush(RGB(25, 25, 25)));
                EndPaint(hwnd, &ps);
                return 0;
            } break;
#endif
            
            case WM_SIZE: {
                int width = LOWORD(lParam);  // Macro to get the low-order word.
                int height = HIWORD(lParam); // Macro to get the high-order word.
                InvalidateRect(hwnd, NULL, TRUE);
                HY_INFO("Resize: (%d, %d)", width, height);
                hyWindow->width = width;
                hyWindow->height = height;
                if (hyWindow->sizeCallback) {
                    hyWindow->sizeCallback(hyWindow, width, height);
                }
            } break;
            
            // TODO(Alex): Possibily ask for confirmation
            case WM_CLOSE: { // X pressed
                DestroyWindow(hwnd);
            } break;
            
            case WM_DESTROY: { // After the window is removed, before all child windows
                // are destroyed
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
        }
    }
    
    return DefWindowProcA(hwnd, msg, wParam, lParam);
}

//~
// Public facing functions.
//

///
/// Swaps front and back buffers.
///
internal void HY_SwapBuffers(HyWindow* window)
{
    SwapBuffers(window->deviceContext);
}

///
/// Returns true if the window should close, false otherwise.
/// With only a window it means the program should exit too.
///
internal b32 HY_WindowShouldClose(HyWindow* window)
{
    return window->shouldClose;
}

///
/// Toggle window fullscreen.
/// TODO(alex): Maybe we need a better API. On other platform you may not rely
/// on styles to detect being fullscreen.
///
internal void HY_ToggleFullscreen(HyWindow* hyWindow)
{
    HWND hwnd = hyWindow->handle;
    
    DWORD dwStyle = GetWindowLong(hwnd, GWL_STYLE);
    if (dwStyle & WS_OVERLAPPEDWINDOW) { // Enable
        MONITORINFO mi = {sizeof(mi)};
        if (GetWindowPlacement(hwnd, &hyWindow->prevPos) &&
            GetMonitorInfo(MonitorFromWindow(hwnd, MONITOR_DEFAULTTOPRIMARY), &mi)) {
            hyWindow->fullscreen = true;
            
            SetWindowLong(hwnd, GWL_STYLE, dwStyle & ~WS_OVERLAPPEDWINDOW);
            SetWindowPos(hwnd, HWND_TOP, mi.rcMonitor.left, mi.rcMonitor.top, mi.rcMonitor.right - mi.rcMonitor.left,
                         mi.rcMonitor.bottom - mi.rcMonitor.top, SWP_NOOWNERZORDER | SWP_FRAMECHANGED);
            
            // Win32SetBorderlessShadow(hyWindow, false);
            // Win32SetBorderless(hyWindow, false);
        }
    } else { // Disable
        hyWindow->fullscreen = false;
        
        SetWindowLong(hwnd, GWL_STYLE, dwStyle | WS_OVERLAPPEDWINDOW);
        SetWindowPlacement(hwnd, &hyWindow->prevPos);
        SetWindowPos(hwnd, NULL, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_NOOWNERZORDER | SWP_FRAMECHANGED);
        
        // Win32SetBorderlessShadow(hyWindow, true);
        // Win32SetBorderless(hyWindow, true);
    }
}

///
/// @warn Should be called BEFORE HY_CreateWindow();
///
internal void HY_SetWindowSizeCallback(HyWindow* hyWindow, window_size_callback_t callback)
{
    hyWindow->sizeCallback = callback;
}

///
///
///
internal void HY_PollEvents(HyWindow* hyWindow)
{
    MSG message;
    
    // NOTE(alex): GetMessage = blocking, PeekMessage = non blocking
    while (PeekMessageA(&message, 0, 0, 0, PM_REMOVE)) {
        switch (message.message) {
            case WM_QUIT: { // received from PostQuitMessage()
                hyWindow->shouldClose = true;
            } break;
            
            case WM_KEYDOWN: {
                u32 vkCode = (u32)message.wParam;
                if (vkCode == 0x1B) {
                    hyWindow->shouldClose = true;
                }
                if (vkCode == 'F') {
                    HY_ToggleFullscreen(hyWindow);
                }
                if (vkCode == 'S') {
                    Win32SetBorderlessShadow(hyWindow, !hyWindow->borderless_shadow);
                }
                if (vkCode == 'B') {
                    Win32SetBorderless(hyWindow, !hyWindow->borderless);
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
    
    WNDCLASSA window_class = {0};
    window_class.style = CS_OWNDC | CS_HREDRAW | CS_VREDRAW;
    window_class.lpfnWndProc = Win32WndProc;
    window_class.hInstance = GetModuleHandle(NULL);
    window_class.hCursor = LoadCursor(NULL, IDC_ARROW);
    window_class.hIcon = LoadIcon(window_class.hInstance, MAKEINTRESOURCE(101));
    window_class.lpszClassName = "HyperWindowClass";
    window_class.hbrBackground = CreateSolidBrush(RGB(25, 25, 25));
    
    RegisterClassA(&window_class);
    
    hyWindow->windowClass = window_class;
    
#if 0
    HMONITOR monitor = MonitorFromWindow(fakeWND, MONITOR_DEFAULTTONEAREST);
    MONITORINFO info;
    info.cbSize = sizeof(MONITORINFO);
    GetMonitorInfo(monitor, &info);
    int width = info.rcMonitor.right - info.rcMonitor.left;
    int height = info.rcMonitor.bottom - info.rcMonitor.top;
#endif
    
    int width = 1920;
    int height = 1080;
    
#if 0
    if (startMode == HyWindowStartMode_Auto) {
        width /= 2;
        height /= 2;
    }
#endif
    
    hyWindow->handle = CreateWindowA(window_class.lpszClassName, title,           // class name, window name
                                     //(DWORD)StyleAeroBorderless, // style
                                     WS_OVERLAPPEDWINDOW,                         // style
                                     CW_USEDEFAULT, CW_USEDEFAULT, width, height, // x, y, width, height
                                     NULL, NULL,                                  // parent window, menu
                                     window_class.hInstance, hyWindow);           // instance, param
    
    MSG message;
    while (PeekMessageA(&message, 0, 0, 0, PM_REMOVE))
        ;
    
    hyWindow->deviceContext = GetDC(hyWindow->handle);
    
    if (!fglLoadOpenGL(false)) {
        HY_FATAL("Failed to load OpenGL!");
    }
    
    HY_INFO("Loaded OpenGL");
    
    fglOpenGLContextCreationParameters contextCreationParams = {0};
    contextCreationParams.windowHandle.win32.deviceContext = hyWindow->deviceContext;
    
    // Create context and load opengl functions
    if (!fglCreateOpenGLContext(&contextCreationParams, &hyWindow->glContext)) {
        HY_FATAL("Failed to load OpenGL functions!");
    }
    
    fglLoadOpenGLFunctions();
    
    HY_INFO("Loaded OpenGL functions");
    
    // Load GL extensions list
    const char* extensions = (const char*)glGetString(GL_EXTENSIONS);
    size_t      ext_string_length = strlen(extensions) + 1;
    g_GlExtension = malloc(sizeof(char) * ext_string_length);
    memcpy((void*)g_GlExtension, extensions, ext_string_length);
    HY_TRACE(g_GlExtension);
    
    hyWindow->glVendor = (const char*)glGetString(GL_VENDOR);
    hyWindow->glRenderer = (const char*)glGetString(GL_RENDERER);
    hyWindow->glVersion = (const char*)glGetString(GL_VERSION);
    hyWindow->glGLSL = (const char*)glGetString(GL_SHADING_LANGUAGE_VERSION);
    
    hyWindow->borderless = true;
    hyWindow->borderless_resize = true;
    hyWindow->borderless_drag = true;
    hyWindow->borderless_shadow = true;
    
    // Win32SetBorderlessShadow(hyWindow, true);
    // Win32SetBorderless(hyWindow, true);
    
    if (startMode == HyWindowStartMode_Maximized) {
        ShowWindow(hyWindow->handle, SW_MAXIMIZE);
    } else if (startMode == HyWindowStartMode_Fullscreen) {
        HY_ToggleFullscreen(hyWindow);
    } else {
        ShowWindow(hyWindow->handle, SW_SHOW);
    }
    
    return HY_NO_ERROR;
}

internal void HY_DestroyWindow(HyWindow* hyWindow)
{
    // TODO(alex): Destroy win32 window
    fglDestroyOpenGLContext(&hyWindow->glContext);
    fglUnloadOpenGL();
}