//
// Created by XW on 2024/5/23.
//
#include "abstractAbility.h"

#include <GL/gl.h>

namespace ULGui::base {

GUint32 AbstractAbility::_count = 0;

void AbstractAbility::setCoordSize(const GUint32& width, const GUint32& height) {
    _coord.setWidth(width);
    _coord.setHeight(height);
}

void AbstractAbility::lineTo(const Point& start, const Point& end) {
    glLineWidth(_penWidth);
    glBegin(GL_LINES);
    glColor4f(_penColor.red, _penColor.green, _penColor.blue, _penColor.alpha);
    auto temp = _coord.toViewPort(start);
    glVertex3f(temp.x, temp.y, temp.z);
    temp = _coord.toViewPort(end);
    glVertex3f(temp.x, temp.y, temp.z);
    glEnd();
}

void AbstractAbility::lineTo(const Coord& start, const Coord& end) {
    glLineWidth(_penWidth);
    glBegin(GL_LINES);
    glColor4f(_penColor.red, _penColor.green, _penColor.blue, _penColor.alpha);
    auto temp = start.toViewPort();
    glVertex3f(temp.x, temp.y, temp.z);
    temp = end.toViewPort();
    glVertex3f(temp.x, temp.y, temp.z);
    glEnd();
}

void AbstractAbility::point(const Point& position) {
    glPointSize(_penWidth);
    glBegin(GL_POINTS);
    glColor4f(_penColor.red, _penColor.green, _penColor.blue, _penColor.alpha);
    auto temp = _coord.toViewPort(position);
    glVertex3f(temp.x, temp.y, temp.z);
    glEnd();
}

void AbstractAbility::point(const Coord& position) {
    glPointSize(_penWidth);
    glBegin(GL_POINTS);
    glColor4f(_penColor.red, _penColor.green, _penColor.blue, _penColor.alpha);
    auto temp = position.toViewPort();
    glVertex3f(temp.x, temp.y, temp.z);
    glEnd();
}
} // namespace graphics2D::base
