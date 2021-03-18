#pragma warning(disable : 4389 )
#pragma warning(disable : 4018 )
#pragma warning(disable : 4244 )
#pragma warning(disable : 4700 )

#define MAINICON 101

//#define _WIN32_WINNT 0x0601 // Targets Windows 7 or later
//#include <sdkddkver.h>

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

#if 0
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
//#define NOTEXTMETRIC
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
#endif

#include <windows.h>
#include <windowsx.h>
#include <shellapi.h>
#include <uxtheme.h>
#include <dwmapi.h>
#include <versionhelpers.h>

extern IMAGE_DOS_HEADER __ImageBase;
#define HINST_THISCOMPONENT ((HINSTANCE)&__ImageBase)

#ifndef WM_NCUAHDRAWCAPTION
#define WM_NCUAHDRAWCAPTION (0x00AE)
#endif
#ifndef WM_NCUAHDRAWFRAME
#define WM_NCUAHDRAWFRAME (0x00AF)
#endif

//~
/// Time.
///

global_variable double g_TimerStart = 0.0f;
global_variable double g_TimerFrequency = 0.0f;

/// Initializes the timer.
internal void hy_timer_init()
{
    if (g_TimerFrequency == 0.0f) {
        QueryPerformanceFrequency((LARGE_INTEGER*)&g_TimerFrequency);
    }
    
    QueryPerformanceCounter((LARGE_INTEGER*)&g_TimerStart);
}

/// Starts the timer.
internal void hy_timer_reset()
{
    QueryPerformanceCounter((LARGE_INTEGER*)&g_TimerStart);
}

/// Returns elapsed microseconds since timer start.
internal double hy_timer_get_microseconds()
{
    double counter;
    QueryPerformanceCounter((LARGE_INTEGER*)&counter);
    counter = (counter - g_TimerStart) / g_TimerFrequency;
    return counter;
}

/// Returns elapsed milliseconds since timer start.
internal double hy_timer_get_milliseconds()
{
    double counter;
    QueryPerformanceCounter((LARGE_INTEGER*)&counter);
    float ms = (float)(1000.0f * (counter - g_TimerStart) / g_TimerFrequency);
    return ms;
}

internal void hy_sleep(uint8_t ms)
{
    Sleep(ms);
}

//~
// Metrics
//

internal float hy_calc_cpu_load(unsigned long long idleTicks, unsigned long long totalTicks)
{
    local_persist unsigned long long _previousTotalTicks = 0;
    local_persist unsigned long long _previousIdleTicks = 0;
    
    unsigned long long totalTicksSinceLastTime = totalTicks - _previousTotalTicks;
    unsigned long long idleTicksSinceLastTime = idleTicks - _previousIdleTicks;
    
    float ret = 1.0f - ((totalTicksSinceLastTime > 0) ? ((float)idleTicksSinceLastTime) / totalTicksSinceLastTime : 0);
    
    _previousTotalTicks = totalTicks;
    _previousIdleTicks = idleTicks;
    return ret;
}

internal unsigned long long hy_file_time_to_int64(FILETIME ft)
{
    return (((unsigned long long)(ft.dwHighDateTime)) << 32) | ((unsigned long long)ft.dwLowDateTime);
}

// Returns 1.0f for "CPU fully pinned", 0.0f for "CPU idle", or somewhere in
// between You'll need to call this at regular intervals, since it measures the
// load between the previous call and the current one.  Returns -1.0 on error.
internal float hy_get_cpu_load()
{
    FILETIME idleTime, kernelTime, userTime;
    return GetSystemTimes(&idleTime, &kernelTime, &userTime)
        ? hy_calc_cpu_load(hy_file_time_to_int64(idleTime), hy_file_time_to_int64(kernelTime) + hy_file_time_to_int64(userTime))
        : -1.0f;
}

//~
// Memory
//

internal void* _hy_malloc(size_t nbytes, char* funcName)
{
    //HY_INFO("%s \tmalloc(%d)", funcName, nbytes);
    return malloc(nbytes);
}

internal void* _hy_calloc(size_t count, size_t nbytes, char* funcName)
{
    //HY_INFO("%s \tcalloc()", funcName);
    return calloc(count, nbytes);
}

internal void* _hy_realloc(void* p, size_t nbytes, char* funcName)
{
    //HY_INFO("%s \trealloc", funcName);
    return realloc(p, nbytes);
}

internal void _hy_free(void* p, char* funcName)
{
    //HY_INFO("%s \tfree()", funcName);
    free(p);
}

//~
// Window.
//

// TODO(alex): When resizing with titlebar from maximized there is a little border visible till you release the mouse button.
// TODO(alex): Implement drawing while resizing or at least not recreate the framebuffer untile resizing is done.
// TODO(alex): Reimplement alt+space from code originally used
// TODO(alex): Reimplement correct set title from code originally used
// TODO(alex): Reimplement correct set icon from code originally used

typedef struct HyWindow HyWindow;

typedef void (*window_size_callback_t)(HyWindow*, unsigned int, unsigned int);

/// Platform indipendent window.
struct HyWindow {
    bool theme_enabled;
	bool composition_enabled;
    
    RECT rgn;
    b32                    shouldClose;
    HWND                   window;
    HDC                    deviceContext;
    HGLRC                  renderingContext;
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

internal void hy_set_fullscreen(HyWindow* hyWindow, bool enable)
{
    HWND hwnd = hyWindow->window;
    hyWindow->fullscreen = enable;
    
    DWORD dwStyle = GetWindowLong(hwnd, GWL_STYLE);
    if (enable) { // Enable
        MONITORINFO mi = {sizeof(mi)};
        if (GetWindowPlacement(hwnd, &hyWindow->prevPos) &&
            GetMonitorInfo(MonitorFromWindow(hwnd, MONITOR_DEFAULTTOPRIMARY), &mi)) {
            
            SetWindowLong(hwnd, GWL_STYLE, dwStyle & ~WS_OVERLAPPEDWINDOW);
            SetWindowPos(hwnd, HWND_TOP, mi.rcMonitor.left, mi.rcMonitor.top, mi.rcMonitor.right - mi.rcMonitor.left,
                         mi.rcMonitor.bottom - mi.rcMonitor.top, SWP_NOOWNERZORDER | SWP_FRAMECHANGED);
        }
    } else { // Disable
        SetWindowLong(hwnd, GWL_STYLE, dwStyle | WS_OVERLAPPEDWINDOW);
        hyWindow->prevPos.ptMaxPosition.x /= 2;
        hyWindow->prevPos.ptMaxPosition.y /= 2;
        SetWindowPlacement(hwnd, &hyWindow->prevPos);
        SetWindowPos(hwnd, NULL, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_NOOWNERZORDER | SWP_FRAMECHANGED);
    }
}

internal void hy_toggle_fullscreen(HyWindow* hyWindow)
{
    hy_set_fullscreen(hyWindow, !hyWindow->fullscreen);
}

internal void win32_update_region(HyWindow *data)
{
	RECT old_rgn = data->rgn;
    
	if (IsMaximized(data->window)) {
		WINDOWINFO wi = { .cbSize = sizeof wi };
		GetWindowInfo(data->window, &wi);
        
		/* For maximized windows, a region is needed to cut off the non-client
		   borders that hang over the edge of the screen */
		data->rgn = (RECT) {
			.left = wi.rcClient.left - wi.rcWindow.left,
			.top = wi.rcClient.top - wi.rcWindow.top,
			.right = wi.rcClient.right - wi.rcWindow.left,
			.bottom = wi.rcClient.bottom - wi.rcWindow.top,
		};
	} else if (!data->composition_enabled) {
		/* For ordinary themed windows when composition is disabled, a region
		   is needed to remove the rounded top corners. Make it as large as
		   possible to avoid having to change it when the window is resized. */
		data->rgn = (RECT) {
			.left = 0,
			.top = 0,
			.right = 32767,
			.bottom = 32767,
		};
	} else {
		/* Don't mess with the region when composition is enabled and the
		   window is not maximized, otherwise it will lose its shadow */
		data->rgn = (RECT) { 0 };
	}
    
	/* Avoid unnecessarily updating the region to avoid unnecessary redraws */
	if (EqualRect(&data->rgn, &old_rgn))
		return;
	/* Treat empty regions as NULL regions */
	if (EqualRect(&data->rgn, &(RECT) { 0 }))
		SetWindowRgn(data->window, NULL, TRUE);
	else
		SetWindowRgn(data->window, CreateRectRgnIndirect(&data->rgn), TRUE);
}

internal void win32_handle_nccreate(HWND window, CREATESTRUCTW *cs)
{
	struct window *data = cs->lpCreateParams;
	SetWindowLongPtrW(window, GWLP_USERDATA, (LONG_PTR)data);
}

internal void win32_handle_compositionchanged(HyWindow *data)
{
	BOOL enabled = FALSE;
	DwmIsCompositionEnabled(&enabled);
	data->composition_enabled = enabled;
    
	if (enabled) {
		/* The window needs a frame to show a shadow, so give it the smallest
		   amount of frame possible */
		DwmExtendFrameIntoClientArea(data->window, &(MARGINS) { 0, 0, 1, 0 });
		DwmSetWindowAttribute(data->window, DWMWA_NCRENDERING_POLICY,
		                      &(DWORD) { DWMNCRP_ENABLED }, sizeof(DWORD));
	}
    
	win32_update_region(data);
}

internal bool win32_has_autohide_appbar(UINT edge, RECT mon)
{
	if (IsWindows8Point1OrGreater()) {
		return SHAppBarMessage(ABM_GETAUTOHIDEBAREX, &(APPBARDATA) {
                                   .cbSize = sizeof(APPBARDATA),
                                   .uEdge = edge,
                                   .rc = mon,
                               });
	}
    
	/* Before Windows 8.1, it was not possible to specify a monitor when
	   checking for hidden appbars, so check only on the primary monitor */
	if (mon.left != 0 || mon.top != 0)
		return false;
	return SHAppBarMessage(ABM_GETAUTOHIDEBAR, &(APPBARDATA) {
                               .cbSize = sizeof(APPBARDATA),
                               .uEdge = edge,
                           });
}

internal int win32_is_maximized(HWND hwnd)
{
    WINDOWPLACEMENT placement;
    if (!GetWindowPlacement(hwnd, &placement)) {
        return false;
    }
    
    return placement.showCmd == SW_MAXIMIZE;
}

internal void win32_handle_nccalcsize(HyWindow *data, WPARAM wparam, LPARAM lparam)
{
	union {
		LPARAM lparam;
		RECT* rect;
	} params = { .lparam = lparam };
    
	/* DefWindowProc must be called in both the maximized and non-maximized
	   cases, otherwise tile/cascade windows won't work */
	RECT nonclient = *params.rect;
	DefWindowProcW(data->window, WM_NCCALCSIZE, wparam, params.lparam);
	RECT client = *params.rect;
    
	if (IsMaximized(data->window)) {
		WINDOWINFO wi = { .cbSize = sizeof wi };
		GetWindowInfo(data->window, &wi);
        
		/* Maximized windows always have a non-client border that hangs over
		   the edge of the screen, so the size proposed by WM_NCCALCSIZE is
		   fine. Just adjust the top border to remove the window title. */
		*params.rect = (RECT) {
			.left = client.left,
			.top = nonclient.top + wi.cyWindowBorders,
			.right = client.right,
			.bottom = client.bottom,
		};
        
		HMONITOR mon = MonitorFromWindow(data->window, MONITOR_DEFAULTTOPRIMARY);
		MONITORINFO mi = { .cbSize = sizeof mi };
		GetMonitorInfoW(mon, &mi);
        
		/* If the client rectangle is the same as the monitor's rectangle,
		   the shell assumes that the window has gone fullscreen, so it removes
		   the topmost attribute from any auto-hide appbars, making them
		   inaccessible. To avoid this, reduce the size of the client area by
		   one pixel on a certain edge. The edge is chosen based on which side
		   of the monitor is likely to contain an auto-hide appbar, so the
		   missing client area is covered by it. */
		if (EqualRect(params.rect, &mi.rcMonitor)) {
			if (win32_has_autohide_appbar(ABE_BOTTOM, mi.rcMonitor))
				params.rect->bottom--;
			else if (win32_has_autohide_appbar(ABE_LEFT, mi.rcMonitor))
				params.rect->left++;
			else if (win32_has_autohide_appbar(ABE_TOP, mi.rcMonitor))
				params.rect->top++;
			else if (win32_has_autohide_appbar(ABE_RIGHT, mi.rcMonitor))
				params.rect->right--;
		}
	} else {
		/* For the non-maximized case, set the output RECT to what it was
		   before WM_NCCALCSIZE modified it. This will make the client size the
		   same as the non-client size. */
		*params.rect = nonclient;
	}
}

internal LRESULT win32_handle_nchittest(HyWindow *data, int x, int y)
{
    // Exit early or we will get artifacts where window stays fullscreen but is also draggable.
    if (data->fullscreen) {
        return HTCLIENT;
    }
    
#if 0
	if (win32_is_maximized(data->window)) {
        return HTNOWHERE;
    }
#endif
    
	POINT mouse = { x, y };
	ScreenToClient(data->window, &mouse);
    
	/* The horizontal frame should be the same size as the vertical frame,
	   since the NONCLIENTMETRICS structure does not distinguish between them */
	int frame_size = GetSystemMetrics(SM_CXFRAME) + GetSystemMetrics(SM_CXPADDEDBORDER);
	/* The diagonal size handles are wider than the frame */
	int diagonal_width = frame_size * 2 + GetSystemMetrics(SM_CXBORDER);
    
	if (mouse.y < frame_size) {
		if (mouse.x < diagonal_width)
			return HTTOPLEFT;
		if (mouse.x >= data->width - diagonal_width)
			return HTTOPRIGHT;
        if (win32_is_maximized(data->window)) {
            return HTCAPTION;
        }
		return HTTOP;
	}
    
	if (mouse.y >= data->height - frame_size) {
		if (mouse.x < diagonal_width)
			return HTBOTTOMLEFT;
		if (mouse.x >= data->width - diagonal_width)
			return HTBOTTOMRIGHT;
		return HTBOTTOM;
	}
    
	if (mouse.x < frame_size)
		return HTLEFT;
	if (mouse.x >= data->width - frame_size)
		return HTRIGHT;
    
    // NOTE(alex): Test if we hit the title bar
    if ((mouse.x < data->width) &&
        (mouse.x >= 0) &&
        (mouse.y < (50)) &&
        (mouse.y >= 0)) {
        return HTCAPTION;
    }
    
	return HTCLIENT;
}

internal void win32_handle_paint(HyWindow *data)
{
    PAINTSTRUCT ps;
	HDC dc = BeginPaint(data->window, &ps);
	//HBRUSH bb = CreateSolidBrush(RGB(0, 255, 0));
	HBRUSH bb = CreateSolidBrush(RGB(25, 25, 25));
    
	/* Draw a rectangle on the border of the client area for testing */
	FillRect(dc, &(RECT) { 0, 0, 1, data->height }, bb);
	FillRect(dc, &(RECT) { 0, 0, data->width, 1 }, bb);
	FillRect(dc, &(RECT) { data->width - 1, 0, data->width, data->height }, bb);
	FillRect(dc, &(RECT) { 0, data->height - 1, data->width, data->height }, bb);
    
	DeleteObject(bb);
	EndPaint(data->window, &ps);
}

internal void win32_handle_themechanged(HyWindow *data)
{
	data->theme_enabled = IsThemeActive();
}

internal void win32_handle_windowposchanged(HyWindow *data, const WINDOWPOS *pos)
{
	RECT client;
	GetClientRect(data->window, &client);
	unsigned old_width = data->width;
	unsigned old_height = data->height;
	data->width = client.right;
	data->height = client.bottom;
	bool client_changed = data->width != old_width || data->height != old_height;
    
    if (client_changed) {
        HY_INFO("Resize: (%d, %d)", data->width, data->height);
        if (data->sizeCallback) {
            data->sizeCallback(data, data->width, data->height);
        }
    }
    
	if (client_changed || (pos->flags & SWP_FRAMECHANGED))
		win32_update_region(data);
    
	if (client_changed) {
		/* Invalidate the changed parts of the rectangle drawn in WM_PAINT */
		if (data->width > old_width) {
			InvalidateRect(data->window, &(RECT) {
                               old_width - 1, 0, old_width, old_height
                           }, TRUE);
		} else {
			InvalidateRect(data->window, &(RECT) {
                               data->width - 1, 0, data->width, data->height
                           }, TRUE);
		}
		if (data->height > old_height) {
			InvalidateRect(data->window, &(RECT) {
                               0, old_height - 1, old_width, old_height
                           }, TRUE);
		} else {
			InvalidateRect(data->window, &(RECT) {
                               0, data->height - 1, data->width, data->height
                           }, TRUE);
		}
	}
}

internal LRESULT win32_handle_message_invisible(HWND window, UINT msg, WPARAM wparam,
                                                LPARAM lparam)
{
	LONG_PTR old_style = GetWindowLongPtrW(window, GWL_STYLE);
    
	/* Prevent Windows from drawing the default title bar by temporarily
	   toggling the WS_VISIBLE style. This is recommended in:
	   https://blogs.msdn.microsoft.com/wpfsdk/2008/09/08/custom-window-chrome-in-wpf/ */
	SetWindowLongPtrW(window, GWL_STYLE, old_style & ~WS_VISIBLE);
	LRESULT result = DefWindowProcW(window, msg, wparam, lparam);
	SetWindowLongPtrW(window, GWL_STYLE, old_style);
    
	return result;
}

static LRESULT CALLBACK win32_wndproc(HWND window, UINT msg, WPARAM wparam, LPARAM lparam)
{
	HyWindow *hyWindow = (void*)GetWindowLongPtrW(window, GWLP_USERDATA);
	if (!hyWindow) {
		/* Due to a longstanding Windows bug, overlapped windows will receive a
		   WM_GETMINMAXINFO message before WM_NCCREATE. This is safe to ignore.
		   It doesn't need any special handling anyway. */
		if (msg == WM_NCCREATE)
			win32_handle_nccreate(window, (CREATESTRUCTW*)lparam);
		return DefWindowProcW(window, msg, wparam, lparam);
	}
    
	switch (msg) {
        case WM_CLOSE: { // X pressed
            // TODO(Alex): Possibily ask for confirmation
            DestroyWindow(window);
            return 0;
        }
        case WM_DESTROY: { // After the window is removed, before all child windows are destroyed
            PostQuitMessage(0);
            return 0;
        }
        case WM_DWMCOMPOSITIONCHANGED: {
            win32_handle_compositionchanged(hyWindow);
            return 0;
        }
        case WM_NCACTIVATE: {
            /* DefWindowProc won't repaint the window border if lParam (normally a
               HRGN) is -1. This is recommended in:
               https://blogs.msdn.microsoft.com/wpfsdk/2008/09/08/custom-window-chrome-in-wpf/ */
            return DefWindowProcW(window, msg, wparam, -1);
        }
        case WM_NCCALCSIZE: {
            win32_handle_nccalcsize(hyWindow, wparam, lparam);
            return 0;
        }
        case WM_NCHITTEST: {
            return win32_handle_nchittest(hyWindow, GET_X_LPARAM(lparam), GET_Y_LPARAM(lparam));
        }
        case WM_NCPAINT: {
            /* Only block WM_NCPAINT when composition is disabled. If it's blocked
               when composition is enabled, the window shadow won't be drawn. */
            if (!hyWindow->composition_enabled)
                return 0;
            break;
        }
        case WM_NCUAHDRAWCAPTION: {
            case WM_NCUAHDRAWFRAME:
            /* These undocumented messages are sent to draw themed window borders.
               Block them to prevent drawing borders over the client area. */
            return 0;
        }
        case WM_PAINT: {
            win32_handle_paint(hyWindow);
            return 0;
        }
        case WM_SETICON:
        case WM_SETTEXT: {
            /* Disable painting while these messages are handled to prevent them
               from drawing a window caption over the client area, but only when
               composition and theming are disabled. These messages don't paint
               when composition is enabled and blocking WM_NCUAHDRAWCAPTION should
               be enough to prevent painting when theming is enabled. */
            if (!hyWindow->composition_enabled && !hyWindow->theme_enabled)
                return win32_handle_message_invisible(window, msg, wparam, lparam);
            break;
        }
        case WM_THEMECHANGED: {
            win32_handle_themechanged(hyWindow);
            break;
        }
        case WM_WINDOWPOSCHANGED: {
            win32_handle_windowposchanged(hyWindow, (WINDOWPOS*)lparam);
            return 0;
        }
        case WM_SYSKEYDOWN:
        case WM_SYSKEYUP:
        case WM_KEYDOWN:
        case WM_KEYUP: {
            HY_FASSERT(!"Keyboard input came in through a non-dispatch message");
        } break;
	}
    
	return DefWindowProcW(window, msg, wparam, lparam);
}

//~
// Public facing functions.
//

///
/// Swaps front and back buffers.
///
internal void hy_swap_buffers(HyWindow* window)
{
    SwapBuffers(window->deviceContext);
}

///
/// Returns true if the window should close, false otherwise.
/// With only a window it means the program should exit too.
///
internal b32 hy_window_should_close(HyWindow* window)
{
    return window->shouldClose;
}

///
/// Toggle window fullscreen.
/// TODO(alex): Maybe we need a better API. On other platform you may not rely
/// on styles to detect being fullscreen.
///

///
/// Should be called BEFORE HY_CreateWindow();
///
internal void hy_set_window_size_callback(HyWindow* hyWindow, window_size_callback_t callback)
{
    hyWindow->sizeCallback = callback;
}

///
///
///
internal void hy_poll_events(HyWindow* hyWindow)
{
    MSG message;
    
    // NOTE(alex): GetMessage = blocking, PeekMessage = non blocking
    while (PeekMessageA(&message, 0, 0, 0, PM_REMOVE)) {
        switch (message.message) {
            case WM_QUIT: { // received from PostQuitMessage()
                hyWindow->shouldClose = true;
            } break;
            
            case WM_SYSKEYDOWN: break;
            case WM_SYSKEYUP: break;
            case WM_KEYUP: break;
            case WM_KEYDOWN: {
                u32 vkCode = (u32)message.wParam;
                if (vkCode == 0x1B) {
                    hyWindow->shouldClose = true;
                }
                if (vkCode == 'F') {
                    hy_toggle_fullscreen(hyWindow);
                }
            } break;
            
            case WM_CHAR: {
                
            } break;
            
            case WM_LBUTTONDOWN: {
                POINTS p = MAKEPOINTS(message.lParam);
                HY_INFO("%d %d", p.x, p.y);
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
/// The window has no borders, but it behaves like a native Windows window.
///
internal int hy_window_create_borderless(HyWindow* hyWindow, HyWindowStartMode startMode, const char* title)
{
    if (!g_HyperEngineInitialized) {
        return HY_NOT_INITIALIZED;
    }
    
    ATOM cls = RegisterClassExW(&(WNDCLASSEXW) {
                                    .cbSize = sizeof(WNDCLASSEXW),
                                    //.style = CS_OWNDC | CS_HREDRAW | CS_VREDRAW,
                                    .lpfnWndProc = win32_wndproc,
                                    .hInstance = HINST_THISCOMPONENT,
                                    //.hInstance = GetModuleHandle(NULL),
                                    .hCursor = LoadCursor(NULL, IDC_ARROW),
                                    .hIcon = LoadIcon(HINST_THISCOMPONENT, MAKEINTRESOURCE(101)),
                                    .hbrBackground = CreateSolidBrush(RGB(25, 25, 25)),
                                    //.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1),
                                    .lpszClassName = L"HyperWindowClass"
                                });
    
    hyWindow->window = CreateWindowExW(WS_EX_APPWINDOW | WS_EX_LAYERED,
                                       (LPWSTR)MAKEINTATOM(cls),
                                       L"Hyper",
                                       WS_OVERLAPPEDWINDOW | WS_SIZEBOX,        // style
                                       CW_USEDEFAULT, CW_USEDEFAULT, 1280, 720, // x, y, w, h
                                       //CW_USEDEFAULT, CW_USEDEFAULT, 2560, 1440, // x, y, w, h
                                       NULL, NULL,                              // parent window, menu
                                       HINST_THISCOMPONENT, hyWindow);          // instance, param
    
    hyWindow->deviceContext = GetDC(hyWindow->window);
    
    if (!fglLoadOpenGL(false)) {
        HY_FATAL("Failed to load OpenGL!");
    } else {
        HY_INFO("Loaded OpenGL");
    }
    
    // Create context and load opengl functions
    fglOpenGLContextCreationParameters contextCreationParams = {0};
    contextCreationParams.windowHandle.win32.deviceContext = hyWindow->deviceContext;
    if (!fglCreateOpenGLContext(&contextCreationParams, &hyWindow->glContext)) {
        HY_FATAL("Failed to load OpenGL functions!");
    }
    
    fglLoadOpenGLFunctions();
    
    HY_INFO("Loaded OpenGL functions");
    
    // Load GL extensions list
    const char* extensions = (const char*)glGetString(GL_EXTENSIONS);
    size_t      ext_string_length = strlen(extensions) + 1;
    g_GlExtension = hy_malloc(sizeof(char) * ext_string_length);
    memcpy((void*)g_GlExtension, extensions, ext_string_length);
    HY_TRACE(g_GlExtension);
    
    hyWindow->glVendor = (const char*)glGetString(GL_VENDOR);
    hyWindow->glRenderer = (const char*)glGetString(GL_RENDERER);
    hyWindow->glVersion = (const char*)glGetString(GL_VERSION);
    hyWindow->glGLSL = (const char*)glGetString(GL_SHADING_LANGUAGE_VERSION);
    
    /* Make the window a layered window so the legacy GDI API can be used to
   draw to it without messing up the area on top of the DWM frame. Note:
   This is not necessary if other drawing APIs are used, eg. GDI+, OpenGL,
   Direct2D, Direct3D, DirectComposition, etc. */
	SetLayeredWindowAttributes(hyWindow->window, RGB(255, 0, 255), 0, LWA_COLORKEY);
    
    win32_handle_compositionchanged(hyWindow);
	win32_handle_themechanged(hyWindow);
    
    // TODO(alex): When launching in fullscreen, returning to borderless covers the whole screen.
    if (startMode == HyWindowStartMode_Maximized) {
        ShowWindow(hyWindow->window, SW_MAXIMIZE);
    } else if (startMode == HyWindowStartMode_Fullscreen) {
        ShowWindow(hyWindow->window, SW_SHOWDEFAULT);
        hy_set_fullscreen(hyWindow, true);
    } else {
        ShowWindow(hyWindow->window, SW_SHOWDEFAULT);
    }
    
    // Issue paint call
    UpdateWindow(hyWindow->window);
    
    return HY_NO_ERROR;
}

internal void hy_window_destroy_borderless(HyWindow* hyWindow)
{
    DestroyWindow(hyWindow->window);
    fglDestroyOpenGLContext(&hyWindow->glContext);
    fglUnloadOpenGL();
}

///
/// Creates a new Window with a modern OpenGL context.
/// The window has no borders, but it behaves like a native Windows window.
///
internal int hy_window_create(HyWindow* hyWindow, HyWindowStartMode startMode, const char* title)
{
    if (!g_HyperEngineInitialized) {
        return HY_NOT_INITIALIZED;
    }
    
    ATOM cls = RegisterClassExW(&(WNDCLASSEXW) {
                                    .cbSize = sizeof(WNDCLASSEXW),
                                    //.style = CS_OWNDC | CS_HREDRAW | CS_VREDRAW,
                                    .lpfnWndProc = win32_wndproc,
                                    .hInstance = HINST_THISCOMPONENT,
                                    //.hInstance = GetModuleHandle(NULL),
                                    .hCursor = LoadCursor(NULL, IDC_ARROW),
                                    .hIcon = LoadIcon(HINST_THISCOMPONENT, MAKEINTRESOURCE(101)),
                                    .hbrBackground = CreateSolidBrush(RGB(25, 25, 25)),
                                    //.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1),
                                    .lpszClassName = L"HyperWindowClass"
                                });
    
    hyWindow->window = CreateWindowExW(WS_EX_APPWINDOW | WS_EX_LAYERED,
                                       (LPWSTR)MAKEINTATOM(cls),
                                       L"Hyper",
                                       WS_OVERLAPPEDWINDOW | WS_SIZEBOX,        // style
                                       CW_USEDEFAULT, CW_USEDEFAULT, 1280, 720, // x, y, w, h
                                       NULL, NULL,                              // parent window, menu
                                       HINST_THISCOMPONENT, hyWindow);          // instance, param
    
    hyWindow->deviceContext = GetDC(hyWindow->window);
    
    ShowWindow(hyWindow->window, SW_SHOWDEFAULT);
    
    // Issue paint call
    UpdateWindow(hyWindow->window);
    
    return HY_NO_ERROR;
}

internal void hy_window_destroy(HyWindow* hyWindow)
{
    DestroyWindow(hyWindow->window);
}

internal int hy_main(int argc, char* argv[]);

// Subsystem:console
int main(int argc, char* argv[]) {
    hy_main(argc, argv);
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
    int result = hy_main(argc, NULL);
    
    LocalFree(argv); // Free memory allocated for CommandLineToArgvW arguments.
    
    ExitProcess(result);
}