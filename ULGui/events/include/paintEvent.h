//
// Created by XW on 2024/5/29.
//

#ifndef _PAINTEVENT_H
#define _PAINTEVENT_H

#include "event.h"

namespace ULGui::event {

class PaintEvent : public virtual Event{
public:
    PaintEvent() = default;
};

}

#endif // _PAINTEVENT_H
