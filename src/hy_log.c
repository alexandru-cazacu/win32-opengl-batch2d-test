#ifdef HY_ENABLE_LOG

#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN    "\x1b[36m"
#define ANSI_COLOR_BOLD    "\x1b[1m"
#define ANSI_COLOR_RESET   "\x1b[0m"

// Core log macros
#define HY_CORE_TRACE(...)   HY_Log(__VA_ARGS__, __FILE__, __LINE__, ANSI_COLOR_RESET, "TRACE")
#define HY_CORE_INFO(...)    HY_Log(__VA_ARGS__, __FILE__, __LINE__, ANSI_COLOR_BLUE, "INFO")
#define HY_CORE_WARN(...)    HY_Log(__VA_ARGS__, __FILE__, __LINE__, ANSI_COLOR_YELLOW, "WARN")
#define HY_CORE_ERROR(...)   HY_Log(__VA_ARGS__, __FILE__, __LINE__, ANSI_COLOR_RED, "ERROR")
#define HY_CORE_FATAL(...)   HY_Log(__VA_ARGS__, __FILE__, __LINE__, ANSI_COLOR_RED ANSI_COLOR_BOLD, "FATAL")

// Client log macros
#define HY_TRACE(...)   HY_Log(__VA_ARGS__, __FILE__, __LINE__, ANSI_COLOR_RESET, "TRACE")
#define HY_INFO(...)    HY_Log(__VA_ARGS__, __FILE__, __LINE__, ANSI_COLOR_BLUE, "INFO")
#define HY_WARN(...)    HY_Log(__VA_ARGS__, __FILE__, __LINE__, ANSI_COLOR_YELLOW, "WARN")
#define HY_ERROR(...)   HY_Log(__VA_ARGS__, __FILE__, __LINE__, ANSI_COLOR_RED, "ERROR")
#define HY_FATAL(...)   HY_Log(__VA_ARGS__, __FILE__, __LINE__, ANSI_COLOR_RED ANSI_COLOR_BOLD, "FATAL")

static void HY_LogInit();
static void HY_Log(const char* msg, const char* file, int line, const char* color, const char* level);

static BOOL g_LogEnableColors = false;

static void HY_LogInit(BOOL useColors)
{
    g_LogEnableColors = useColors;
    
    if (IsDebuggerPresent()) {
        HY_INFO("[Logger] Debugger present");
    } else {
        HY_INFO("[Logger] Debugger not found. Fallback on stdio.");
        HY_INFO("[Logger] Debugger not found. Assertion will silently crash.");
    }
    
    HY_INFO("[Logger] Initialized.");
}

static void HY_Log(const char* msg, const char* file, int line, const char* color, const char* level)
{
#if 0
    const char* usedColor = g_LogEnableColors ? color : "";
    const char* usedResetColor = g_LogEnableColors ? ANSI_COLOR_RESET : "";
    SYSTEMTIME localTime = {0};
    GetLocalTime(&localTime);
    
    if (IsDebuggerPresent()) {
        OutputDebugStringA(msg);
    } else {
        char fmtMsg[256];;
        char dir[260];
        char drive[260];
        _splitpath_s(file, drive, 260, dir, 260, NULL, 0, NULL, 0);
        int offset = (int)strlen(drive) + (int)strlen(dir);
        int fileLen = (int)strlen(file) - offset;
        snprintf(fmtMsg, 256,
                 "%d:%d:%d %s%s%s %.*s:%d %s\n", // %.*s -> some magic trickstery -> len, str + offset
                 localTime.wHour, localTime.wMinute, localTime.wSecond,
                 usedColor, level, usedResetColor,
                 fileLen, file + offset, line,
                 msg);
        printf(fmtMsg);
    }
#endif
}

#else // HY_ENABLE_LOG
// Core log macros
#define HY_CORE_TRACE(...)
#define HY_CORE_INFO(...)
#define HY_CORE_WARN(...)
#define HY_CORE_ERROR(...)
#define HY_CORE_FATAL(...)

// Client log macros
#define HY_TRACE(...)
#define HY_INFO(...)
#define HY_WARN(...)
#define HY_ERROR(...)
#define HY_FATAL(...)

static void HY_InitLog() {}

#endif // HY_ENABLE_LOG
