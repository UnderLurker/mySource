//
// Created by XW on 2024/5/16.
//

#ifndef _ULGUI_MACRO_H
#define _ULGUI_MACRO_H

#define __ULGUI_DECLARE
#define __ULGUI_2D_DECLARE

// glfw init version
#define WINDOW_VERSION_MAJOR    3
#define WINDOW_VERSION_MINOR    3

// widget default value
#define DEFAULT_WIDGET_WIDTH    200
#define DEFAULT_WIDGET_HEIGHT   150

#define DEFAULT_WIDGET_TITLE    "title"

#define DEFAULT_PEN_WIDTH       1
#define DEFAULT_PEN_COLOR       RGBA_BLACK
#define RGBA_BLACK              GraphicRGBA(0, 0, 0, 255)
#define RGBA_WHITE              GraphicRGBA(255, 255, 255, 255)
#define RGBA_RED                GraphicRGBA(255, 0, 0, 255)
#define RGBA_GREEN              GraphicRGBA(0, 255, 0, 255)
#define RGBA_BLUE               GraphicRGBA(0, 0, 255, 255)
#define RGBA_TRANSPARENT        GraphicRGBA(0, 0, 0, 255)

#define DEFAULT_STIPPLE_FACTOR  2
#define DEFAULT_STIPPLE_PATTERN 0xAAAA

#endif // _ULGUI_MACRO_H
