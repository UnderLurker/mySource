#ifndef _CANVAS_H
#define _CANVAS_H

#include "common/rect.h"

namespace ULGui::canvas {

class Canvas {
public:
    Canvas() = default;
    virtual ~Canvas() = default;
    const RectF size() const { return _size; }

private:
    RectF _size;
};

}

#endif // !_CANVAS_H
