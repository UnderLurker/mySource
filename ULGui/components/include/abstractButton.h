//
// Created by XW on 2024/5/16.
//

#ifndef _ABSTRACTBUTTON_H
#define _ABSTRACTBUTTON_H

#include "abstractComponent.h"
#include "widget.h"

namespace ULGui::components {

class AbstractButton : public virtual window::Widget, public virtual AbstractComponent{
public:
    AbstractButton();
    void print();
private:

};
}

#endif // _ABSTRACTBUTTON_H
