/// @brief Represents a platform-indipendent window.
typedef struct
{
    HWND Handle;
    b32 ShouldClose;
} HyWindow;

b32 HyperWindowShouldClose(HyWindow* window)
{
    return window->ShouldClose;
}

internal void HyperProcessPendingMessages(HyWindow* window)
{
    MSG message;
    
    // NOTE(alex): GetMessage = blocking, PeekMessage = non blocking
    while (PeekMessageA(&message, 0, 0, 0, PM_REMOVE))
    {
        switch(message.message)
        {
            // received from PostQuitMessage()
            case WM_QUIT: // TODO(Alex): try to recreate window
            {
                window->ShouldClose = true;
            } break;
            
            default:
            {
                TranslateMessage(&message);
                DispatchMessage(&message); // Dispatch to Win32WindowCallback()
            }
        }
    }
}

internal LRESULT CALLBACK Win32WindowCallback(HWND window, UINT message, WPARAM wParam, LPARAM lParam)
{
    LRESULT result = 0;
    
    switch(message)
    {
        case WM_ACTIVATEAPP:
        {
#if 0
            HDC deviceContext = GetDC(window);
            Vec2 dimension =  GetWindowDimension();
            Win32InitOpenGL(window);
            Win32DisplayBufferInWindow(deviceContext, dimension.Width, dimension.Height);
#endif
        } break;
        
        // NOTE(alex): X pressed
        case WM_CLOSE: // TODO(Alex): Possibily ask for confirmation
        {
            DestroyWindow(window);
        } break;
        
        // NOTE(alex): After the window is removed, before all child windows are destroyed
        case WM_DESTROY: // TODO(Alex): try to recreate window
        {
            PostQuitMessage(0);
        } break;
        
        case WM_SYSKEYDOWN:
        case WM_SYSKEYUP:
        case WM_KEYDOWN:
        case WM_KEYUP:
        {
            Assert(!"Keyboard input came in through a non-dispatch message");
        } break;
        
        case WM_SIZE:
        {
            //Win32WindowDimension dimension =  GetWindowDimension(window);
            //Win32ResizeDIBSection(&g_Backbuffer, dimension.Width, dimension.Height);
        } break;
        
        default:
        {
            result = DefWindowProcA(window, message, wParam, lParam);
        } break;
    }
    
    return result;
}

HyWindow* hy_create_window(HyWindow* window, const char* title)
{
    if (!g_HyperEngineInitialized)
    {
        g_HyperLastErrorCode = HY_NOT_INITIALIZED;
        return NULL;
    }
    
    //HyWindow* window;
    
    // TODO(alex): Use our memory pool
    //window = (HyWindow*)malloc(sizeof(HyWindow));
    
    WNDCLASSA window_class = {0};
    HINSTANCE hInst = GetModuleHandle(NULL);
    
    window_class.style = CS_OWNDC|CS_HREDRAW|CS_VREDRAW;
    window_class.lpfnWndProc = Win32WindowCallback;
    window_class.hInstance = hInst;
    // window_class.hCursor = ; // TODO(Alex): implement
    window_class.hIcon = LoadIcon(hInst, MAKEINTRESOURCE(101));
    //window_class.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH); // TODO(alex): can do with GL?
    window_class.lpszClassName = "HyperWindowClass";
    
    HWND handle = NULL;
    
    // Register window class.
    if (RegisterClassA(&window_class))
    {
        // Create window.
        handle = CreateWindowExA(0,
                                 window_class.lpszClassName,
                                 title,
                                 WS_VISIBLE|WS_OVERLAPPEDWINDOW,
                                 CW_USEDEFAULT, CW_USEDEFAULT,
                                 CW_USEDEFAULT, CW_USEDEFAULT,
                                 0, 0, hInst, 0);
        
        if(!handle)
        {
            g_HyperLastErrorCode = HY_PLATFORM_ERROR;
            return NULL;
        }
    }
    
    window->Handle = handle;
    window->ShouldClose = false;
    
    return window;
}