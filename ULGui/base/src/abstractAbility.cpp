//
// Created by XW on 2024/5/23.
//
#include "abstractAbility.h"

#include <GL/gl.h>

namespace ULGui::base {
namespace {

void putpixel(const Point& center, int32_t x, int32_t y, const Coord& coord) {
    auto temp = coord.toViewPort({center.x + x, center.y + y});
    glVertex3f(temp.x, temp.y, temp.z);
}

void Circle(const Point& center, int32_t x, int32_t y, const Coord& coord) {
    putpixel(center, x, y, coord);
    putpixel(center, y, x, coord);
    putpixel(center, -y, x, coord);
    putpixel(center, -x, y, coord);
    putpixel(center, -x, -y, coord);
    putpixel(center, -y, -x, coord);
    putpixel(center, y, -x, coord);
    putpixel(center, x, -y, coord);
}

void MidBresenhamCircle(const Point& center, int32_t r, const Coord& coord) {
    int32_t x = 0;
    int32_t y = r;
    int32_t d = 1 - r;
    while (x <= y) {
        Circle(center, x, y, coord);
        if (d < 0) d += 2 * x + 3;
        else d += 2 * (x - y--) + 5;
        x++;
    }
}
} // namespace

GUint32 AbstractAbility::_count = 0;

void AbstractAbility::setCoordSize(const GInt32& width, const GInt32& height) {
    _coord.setWidth(width);
    _coord.setHeight(height);
}

void AbstractAbility::lineTo(const Coord& start, const Coord& end) { lineTo({start.position(), end.position()}); }

void AbstractAbility::lineTo(const std::vector<Point>& pointList) {
    updateStyle();
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

void AbstractAbility::circle(const Point& center, int32_t radius) {
    auto temp = _coord.toViewPort({center.x, center.y});
    updateStyle();
    glPointSize(static_cast<float>(radius) * 2);
    glBegin(GL_POINTS);
    // MidBresenhamCircle(center, radius, _coord);
    glVertex3f(temp.x, temp.y, temp.z);
    glEnd();

    glPointSize((static_cast<float>(radius) - 100.0) * 2);
    glBegin(GL_POINTS);
    glColor3f(255, 0, 255);
    glVertex3f(temp.x +0.5, temp.y, temp.z);
    glEnd();
}

void AbstractAbility::updateStyle() {
    glLineWidth(_style.width);
    glPointSize(_style.width);
    glColor4f(_style.color.red, _style.color.green, _style.color.blue, _style.color.alpha);
}
} // namespace ULGui::base