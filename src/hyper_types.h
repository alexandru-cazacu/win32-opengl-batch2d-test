#pragma once

/// @file
/// @brief Commond type definitions and defines used by the engine.

#include <stdint.h>

typedef int8_t  s8;
typedef int16_t s16;
typedef int32_t s32;
typedef int64_t s64;

typedef uint8_t  u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef float r32;
typedef double r64;

typedef int b32;

#define true 1
#define false 0

#define Pi32 3.14159265359f

#define internal static
#define local_persist static
#define global_variable static

#define HY_MOUSE_BUTTON_1   0
#define HY_MOUSE_BUTTON_2   1
#define HY_MOUSE_BUTTON_3   2
#define HY_MOUSE_BUTTON_4   3
#define HY_MOUSE_BUTTON_5   4
#define HY_MOUSE_BUTTON_LAST   HY_MOUSE_BUTTON_5
#define HY_MOUSE_BUTTON_LEFT   HY_MOUSE_BUTTON_1
#define HY_MOUSE_BUTTON_RIGHT   HY_MOUSE_BUTTON_2
#define HY_MOUSE_BUTTON_MIDDLE   HY_MOUSE_BUTTON_3

#if HY_SLOW
#define Assert(expression) if (!(expression)) { *(int*)0 = 0; }
#else
#define Assert(expression)
#endif

#define ArrayCount(Array) (sizeof(Array) / sizeof((Array)[0]))

#define Kilobytes(value) ((value) * 1024)
#define Megabytes(value) (Kilobytes(value) * 1024)
#define Gigabytes(value) (Megabytes(value) * 1024)
#define Terabytes(value) (Gigabytes(value) * 1024)

inline u32 SafeTruncateU64(u64 value)
{
    Assert(value < 0xffffffff);
    u32 result = (u32)value;
    return result;
}

#define HY_NO_ERROR   0
#define HY_NOT_INITIALIZED   0x00010001
#define HY_PLATFORM_ERROR   0x00010008