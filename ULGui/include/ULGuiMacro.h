//
// Created by XW on 2024/5/16.
//

#ifndef _ULGUI_MACRO_H
#define _ULGUI_MACRO_H

#define __ULGUI_DECLARE
#define __ULGUI_2D_DECLARE

#define CHECK_NULL_VOID(ptr)        \
    do {                            \
        if (ptr == nullptr) return; \
    } while (false);

#define CHECK_NULL_RETURN(ptr, value)     \
    do {                                  \
        if (ptr == nullptr) return value; \
    } while (false);

// glfw init version
#define WINDOW_VERSION_MAJOR     3
#define WINDOW_VERSION_MINOR     3

// 抗锯齿：默认 framebuffer 的多重采样数（glfwWindowHint(GLFW_SAMPLES, ...)）
#define DEFAULT_MSAA_SAMPLES     4

// widget default value
#define DEFAULT_WIDGET_WIDTH     200
#define DEFAULT_WIDGET_HEIGHT    150

#define DEFAULT_WIDGET_TITLE     "title"

#define DEFAULT_PEN_WIDTH        1
#define DEFAULT_PEN_COLOR        RGBA::BLACK

#define DEFAULT_STIPPLE_FACTOR   2
#define DEFAULT_STIPPLE_PATTERN  0xAAAA

#define DEFAULT_CIRCLE_PRECISION 0.5f
#define DEFAULT_RGBA_COLOR       0xFFFFFFFF

#ifdef UTIL_DEBUG
#include <cassert>
#define ULGUI_ASSERT(info) assert(info);
#else
#define ULGUI_ASSERT(info) static_cast<void*>(0);
#endif

#endif // _ULGUI_MACRO_H
