#ifndef _CANVAS_MACRO_H
#define _CANVAS_MACRO_H

#define DECLARE_MOVE_COPY(className)                  \
    className(const className&)            = default; \
    className(className&&)                 = default; \
    className& operator=(const className&) = default; \
    className& operator=(className&&)      = default;

#define DECLARE_MOVE(className)                  \
    className(className&&)            = default; \
    className& operator=(className&&) = default;

#define DECLARE_COPY(className)                       \
    className(const className&)            = default; \
    className& operator=(const className&) = default;

#endif // !_CANVAS_MACRO_H
