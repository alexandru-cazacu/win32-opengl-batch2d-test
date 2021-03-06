#ifdef HY_ENABLE_LOG

#define LOG_USE_COLOR
#include <log.c/log.c>

// Core log macros
#define HY_CORE_TRACE(...) log_trace(__VA_ARGS__)
#define HY_CORE_INFO(...)  log_info(__VA_ARGS__)
#define HY_CORE_WARN(...)  log_warn(__VA_ARGS__)
#define HY_CORE_ERROR(...) log_error(__VA_ARGS__)
#define HY_CORE_FATAL(...) log_fatal(__VA_ARGS__)

// Client log macros
#define HY_TRACE(...) log_trace(__VA_ARGS__)
#define HY_INFO(...)  log_info(__VA_ARGS__)
#define HY_WARN(...)  log_warn(__VA_ARGS__)
#define HY_ERROR(...) log_error(__VA_ARGS__)
#define HY_FATAL(...) log_fatal(__VA_ARGS__)

internal void hy_log_init();
internal void hy_log(const char* msg, const char* file, int line, const char* color, const char* level);

internal void hy_log_init()
{
  log_set_level(LOG_INFO);

  // TODO(alex): Fix
#if 0
    if (IsDebuggerPresent()) {
        HY_INFO("Debugger present");
    } else {
        HY_INFO("Debugger not found. Fallback on stdio.");
        HY_INFO("Debugger not found. Assertion will silently crash.");
    }
#endif

  HY_INFO("[Logger] Initialized.");
}

internal void hy_log(const char* msg, const char* file, int line, const char* color, const char* level)
{
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

internal void hy_log_init()
{
}

#endif // HY_ENABLE_LOG
