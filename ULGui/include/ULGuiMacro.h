//
// Created by XW on 2024/5/16.
//

#ifndef _GRAPHIC2DMACRO_H
#define _GRAPHIC2DMACRO_H

#define __GRAPHIC_DECLARE
#define __GRAPHIC_2D_DECLARE

// glfw init version
#define WINDOW_VERSION_MAJOR 3
#define WINDOW_VERSION_MINOR 3

// widget default value
#define WIDGET_WIDTH         200
#define WIDGET_HEIGHT        150

#define WIDGET_TITLE         "title"

#define DEFAULT_PEN_WIDTH    1
#define DEFAULT_PEN_COLOR    GraphicRGBA(255, 255, 255, 255)
#define BLACK                GraphicRGBA(0, 0, 0, 255)
#define WHITE                GraphicRGBA(255, 255, 255, 255)
#define RED                  GraphicRGBA(255, 0, 0, 255)
#define GREEN                GraphicRGBA(0, 255, 0, 255)
#define BLUE                 GraphicRGBA(0, 0, 255, 255)
#define TRANSPARENT          GraphicRGBA(0, 0, 0, 255)

#endif // _GRAPHIC2DMACRO_H
