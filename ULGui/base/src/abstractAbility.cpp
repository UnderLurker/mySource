//
// Created by XW on 2024/5/23.
//
#include "abstractAbility.h"

#include <cmath>
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

double floatToPI(float value) { return (value / 180.0) * M_PI; }
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

void AbstractAbility::circle(const Point& center, double radius, bool fill) {
    if (radius <= 0) return;
    updateStyle();
    glBegin(fill ? GL_POLYGON : GL_QUAD_STRIP);
    const double angle = asin(_style.width / (radius));
    for (double start = 0; start < floatToPI(360);) {
        Point temp(radius * cos(start) + center.x, radius * sin(start) + center.y);
        Point largeTemp((radius + _style.width) * cos(start) + center.x,
                        (radius + _style.width) * sin(start) + center.y);
        const auto t1 = _coord.toViewPort(temp);
        const auto t2 = _coord.toViewPort(largeTemp);
        glVertex3f(t1.x, t1.y, t1.z);
        glVertex3f(t2.x, t2.y, t2.z);
        start += angle;
    }
    glEnd();
}

void AbstractAbility::arc(const Point& center, double radius, float startAngle, float endAngle) {
    if (radius <= 0) return;
    updateStyle();
    glBegin(GL_QUAD_STRIP);
    const double angle = asin(_style.width / (radius));
    for (double start = floatToPI(startAngle); start < floatToPI(endAngle);) {
        Point temp(radius * cos(start) + center.x, -radius * sin(start) + center.y);
        Point largeTemp((radius + _style.width) * cos(start) + center.x,
                        -(radius + _style.width) * sin(start) + center.y);
        const auto t1 = _coord.toViewPort(temp);
        const auto t2 = _coord.toViewPort(largeTemp);
        glVertex3f(t1.x, t1.y, t1.z);
        glVertex3f(t2.x, t2.y, t2.z);
        start += angle;
    }
    glEnd();
}

void AbstractAbility::updateStyle() {
    glLineWidth(_style.width);
    glPointSize(_style.width);
    glColor4f(_style.color.red, _style.color.green, _style.color.blue, _style.color.alpha);
}
} // namespace ULGui::base
