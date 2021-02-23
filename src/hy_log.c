#ifdef HY_ENABLE_LOG
// Core log macros
#define HY_CORE_TRACE(...)   OutputDebugStringA("Core: " __VA_ARGS__)
#define HY_CORE_INFO(...)    OutputDebugStringA("Core: " __VA_ARGS__)
#define HY_CORE_WARN(...)    OutputDebugStringA("Core: " __VA_ARGS__)
#define HY_CORE_ERROR(...)   OutputDebugStringA("Core: " __VA_ARGS__)
#define HY_CORE_FATAL(...)   OutputDebugStringA("Core: " __VA_ARGS__)

// Client log macros
#define HY_TRACE(...)   OutputDebugStringA("00:00:00 TRACE \t" __FILE__ ": " __VA_ARGS__)
#define HY_INFO(...)    OutputDebugStringA(__VA_ARGS__)
#define HY_WARN(...)    OutputDebugStringA(__VA_ARGS__)
#define HY_ERROR(...)   OutputDebugStringA(__VA_ARGS__)
#define HY_FATAL(...)   OutputDebugStringA(__VA_ARGS__)

static void HY_InitLog()
{
    // TODO(alex): Possibly add configurable logs. Like toggleable colors or format.
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
