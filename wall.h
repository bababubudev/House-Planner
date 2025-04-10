#ifndef WALL_H
#define WALL_H

#include <QLine>
#include <QPainter>
#include <QPoint>


class Wall {
public:
    Wall();
    Wall(const QPoint &start, const QPoint &end);

    QPoint startPoint() const;
    QPoint endPoint() const;
    QLine line() const;
    void setStartPoint(const QPoint &point);
    void setEndPoint(const QPoint &point);

    bool isHorizontal() const;
    bool isVertical() const;

    void draw(QPainter &painter) const;
    bool intersects(const QRectF &rect) const;

private:
    QPoint m_startPoint;
    QPoint m_endPoint;
};

#endif // WALL_H
