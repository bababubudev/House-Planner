#include "wall.h"


Wall::Wall() : m_startPoint(0, 0), m_endPoint(0, 0) {}

Wall::Wall(const QPoint &start, const QPoint &end): m_startPoint(start), m_endPoint(end) {}

QPoint Wall::startPoint() const
{
    return m_startPoint;
}

QPoint Wall::endPoint() const
{
    return m_endPoint;
}

QLine Wall::line() const
{
    return QLine(m_startPoint, m_endPoint);
}

void Wall::setStartPoint(const QPoint &point)
{
    m_startPoint = point;
}

void Wall::setEndPoint(const QPoint &point)
{
    m_endPoint = point;
}

bool Wall::isHorizontal() const
{
    // Difference in y value is less than a threshold.
    return qAbs(m_startPoint.y() - m_endPoint.y()) < 5;
}

bool Wall::isVertical() const
{
    // Difference is x value is less than a threshold.
    return qAbs(m_startPoint.x() - m_endPoint.x()) < 5;
}

void Wall::draw(QPainter &painter) const
{
    painter.drawLine(m_startPoint, m_endPoint);
}

bool Wall::intersects(const QRectF &rect) const
{
    QLineF top(rect.topLeft(), rect.topRight());
    QLineF right(rect.topRight(), rect.bottomRight());
    QLineF bottom(rect.bottomRight(), rect.bottomLeft());
    QLineF left(rect.bottomLeft(), rect.topLeft());

    QLineF wallLine(m_startPoint, m_endPoint);

    QPointF intersection;
    if (wallLine.intersects(top, &intersection) == QLineF::BoundedIntersection ||
        wallLine.intersects(right, &intersection) == QLineF::BoundedIntersection ||
        wallLine.intersects(bottom, &intersection) == QLineF::BoundedIntersection ||
        wallLine.intersects(left, &intersection) == QLineF::BoundedIntersection) {
        return true;
    }

    // Check whether the wall is completely inside the rectangle
    return rect.contains(m_startPoint) && rect.contains(m_endPoint);
}
