#if HY_SLOW
#define HY_FASSERT(x)                                                                                                  \
  if (!(x)) {                                                                                                          \
    *(int*)0 = 0;                                                                                                      \
  }
#define HY_ASSERT(x, __VA_ARGS__)                                                                                      \
  if (!(x)) {                                                                                                          \
    if (IsDebuggerPresent())                                                                                           \
      __debugbreak();                                                                                                  \
    else {                                                                                                             \
      HY_ERROR("Assertion failed but no debugger is attached." __VA_ARGS__);                                           \
      *(int*)0 = 0;                                                                                                    \
    }                                                                                                                  \
  }
#else // HY_SLOW
#define HY_FASSERT(...)
#define HY_ASSERT(...)
#endif // HY_SLOW