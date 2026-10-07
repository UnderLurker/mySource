//
// Created by XW on 2024/5/23.
//
#include "abstractAbility.h"

#include "renderer.h"

namespace ULGui::base {

GUint32 AbstractAbility::_count = 0;

void AbstractAbility::setViewPortSize(const GInt32& width, const GInt32& height) {
    base::Renderer::instance().setViewport(width, height);
}

void AbstractAbility::lineTo(const Point& start, const Point& end) { lineTo({start, end}); }

void AbstractAbility::lineTo(const std::vector<Point>& pointList) {
    const size_t n = pointList.size();
    if (n < 2) return;

    auto& renderer    = base::Renderer::instance();
    const RGBA color  = _style.color;
    const float width = _style.width <= 0 ? 1.0f : _style.width;

    switch (_style.mode) {
        case LINE_LOOP:
            for (size_t i = 0; i < n; ++i) {
                const Point& a = pointList[i];
                const Point& b = pointList[(i + 1) % n];
                renderer.drawLine((float)a.x, (float)a.y, (float)b.x, (float)b.y, width, color);
            }
            break;
        case LINE_STRIP:
            for (size_t i = 0; i + 1 < n; ++i) {
                const Point& a = pointList[i];
                const Point& b = pointList[i + 1];
                renderer.drawLine((float)a.x, (float)a.y, (float)b.x, (float)b.y, width, color);
            }
            break;
        case LINE: // 两两成对
        default:
            for (size_t i = 0; i + 1 < n; i += 2) {
                const Point& a = pointList[i];
                const Point& b = pointList[i + 1];
                renderer.drawLine((float)a.x, (float)a.y, (float)b.x, (float)b.y, width, color);
            }
            break;
    }
}

void AbstractAbility::point(const Point& position) {
    base::Renderer::instance().drawPoint((float)position.x, (float)position.y, _style.width, _style.color);
}

void AbstractAbility::point(const Coord& position) { point(position.position()); }

void AbstractAbility::circle(const Point& center, double radius, bool fill) {
    if (radius <= 0) return;
    auto& render = base::Renderer::instance();
    render.drawCircle((float)center.x, (float)center.y, (float)radius, _style.color, fill, _style.width);
}

void AbstractAbility::arc(const Point& center, double radius, float startAngle, float endAngle) {
    if (radius <= 0) return;
    auto& render = base::Renderer::instance();
    render.drawArc((float)center.x, (float)center.y, (float)radius, startAngle, endAngle, _style.width, _style.color);
}

void AbstractAbility::drawRect(const Point& leftTop, double width, double height) {
    if (width < 0 || height < 0) return;
    auto& render = base::Renderer::instance();
    render.drawRect((float)leftTop.x, (float)leftTop.y, (float)width, (float)height, _brush);
}
} // namespace ULGui::base
