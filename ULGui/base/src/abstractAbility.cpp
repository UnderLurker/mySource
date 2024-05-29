//
// Created by XW on 2024/5/23.
//
#include "abstractAbility.h"

#include <GL/gl.h>

namespace ULGui::base {

GUint32 AbstractAbility::_count = 0;

void AbstractAbility::setCoordSize(const GInt32& width, const GInt32& height) {
    _coord.setWidth(width);
    _coord.setHeight(height);
}

void AbstractAbility::lineTo(const Coord& start, const Coord& end) {
    lineTo({start.position(), end.position()});
}

void AbstractAbility::lineTo(const std::vector<Point>& pointList) {
    glLineWidth(_style.width);
    glColor4f(_style.color.red, _style.color.green, _style.color.blue, _style.color.alpha);
    if (_style.type == Stipple) {
        glEnable(GL_LINE_STIPPLE);
        glLineStipple(_style.stippleStyle.factor, _style.stippleStyle.pattern);
    }
    glBegin(_style.mode);
    for (const auto& item : pointList) {
        auto temp = _coord.toViewPort(item);
        glVertex3f(temp.x, temp.y, temp.z);
    }
    glEnd();
    if (_style.type == Stipple) glDisable(GL_LINE_STIPPLE);
}

void AbstractAbility::point(const Point& position) {
    glPointSize(_style.width);
    glColor4f(_style.color.red, _style.color.green, _style.color.blue, _style.color.alpha);
    glBegin(GL_POINTS);
    auto temp = _coord.toViewPort(position);
    glVertex3f(temp.x, temp.y, temp.z);
    glEnd();
}

void AbstractAbility::point(const Coord& position) {
    glPointSize(_style.width);
    glColor4f(_style.color.red, _style.color.green, _style.color.blue, _style.color.alpha);
    glBegin(GL_POINTS);
    auto temp = position.toViewPort();
    glVertex3f(temp.x, temp.y, temp.z);
    glEnd();
}
} // namespace ULGui::base
