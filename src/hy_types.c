#pragma once

/// @file
/// @brief Commond type definitions and defines used by the engine.

#include <stdbool.h>
#include <stdint.h>

#define Pi32 3.14159265359f

#define internal        static /// Unity Build. For static functions.
#define local_persist   static /// Unity Build. For static variables inside functions.
#define global_variable static /// Unity Build. For static variables inside files.

#define HY_MOUSE_BUTTON_1      0
#define HY_MOUSE_BUTTON_2      1
#define HY_MOUSE_BUTTON_3      2
#define HY_MOUSE_BUTTON_4      3
#define HY_MOUSE_BUTTON_5      4
#define HY_MOUSE_BUTTON_LAST   HY_MOUSE_BUTTON_5
#define HY_MOUSE_BUTTON_LEFT   HY_MOUSE_BUTTON_1
#define HY_MOUSE_BUTTON_RIGHT  HY_MOUSE_BUTTON_2
#define HY_MOUSE_BUTTON_MIDDLE HY_MOUSE_BUTTON_3

#define ArrayCount(Array) (sizeof(Array) / sizeof((Array)[0]))

#define Kilobytes(value) ((value)*1024)
#define Megabytes(value) (Kilobytes(value) * 1024)
#define Gigabytes(value) (Megabytes(value) * 1024)
#define Terabytes(value) (Gigabytes(value) * 1024)

// NOTE(alex): Assert that does't log, just crashes.
#if HY_SLOW
#define HY_FASSERT(x)                                                                                                  \
if (!(x)) {                                                                                                          \
*(int*)0 = 0;                                                                                                      \
}
#else
#define HY_FASSERT(x)
#endif

inline uint32_t SafeTruncateU64(uint64_t value)
{
    HY_FASSERT(value < 0xffffffff);
    uint32_t result = (uint32_t)value;
    return result;
}

typedef enum HyError { HY_NO_ERROR, HY_NOT_INITIALIZED, HY_PLATFORM_ERROR } HyError;

#define HY_NO_ERROR        0
#define HY_NOT_INITIALIZED 0x00010001
#define HY_PLATFORM_ERROR  0x00010008

global_variable int         g_HyperEngineInitialized = true;
global_variable int         g_HyperLastErrorCode = 0;
global_variable const char* g_GlExtension = "";
