#ifndef FURNITURE_H
#define FURNITURE_H

#include "wall.h"
#include <QPainter>
#include <QPointF>
#include <QRectF>
#include <QUuid>


enum class FurnitureType {
    Sofa,
    Chair,
    Table
};

class Furniture {
public:
    Furniture();
    Furniture(const QPointF &position, qreal width, qreal height, FurnitureType type);
    virtual ~Furniture();

    QPointF position() const;
    void setPosition(const QPointF &position);

    qreal width() const;
    qreal height() const;

    qreal rotation() const;
    void setRotation(qreal angle);

    FurnitureType type() const;
    QString typeName() const;

    QUuid id() const;

    QRectF boundingRect() const;
    QRectF rotatedBoundingRect() const;

    bool isSelected() const;
    void setSelected(bool selected);

    virtual void draw(QPainter &painter) const;

    bool collidesWith(const Furniture *other) const;
    bool collidesWith(const QList<Wall> &walls) const;
    bool collidesWithAny(const QList<Furniture*> &furniture, const QList<Wall> &walls) const;

    friend QDataStream &operator<<(QDataStream &stream, const Furniture &furniture);
    friend QDataStream &operator>>(QDataStream &stream, Furniture &furniture);

    virtual Furniture *clone() const = 0;

protected:
    QPointF m_position;
    qreal m_width;
    qreal m_height;
    qreal m_rotation;
    FurnitureType m_type;
    QUuid m_id;
    bool m_selected;

    QColor getColorForType() const;
};

class Sofa: public Furniture {
public:
    Sofa();
    Sofa(const QPointF &position);

    void draw(QPainter &painter) const override;
    Furniture *clone() const override;
};

class Chair: public Furniture {
public:
    Chair();
    Chair(const QPointF &position);

    void draw(QPainter &painter) const override;
    Furniture *clone() const override;
};

class Table: public Furniture {
public:
    Table();
    Table(const QPointF &position);

    void draw(QPainter &painter) const override;
    Furniture *clone() const override;
};

#endif // FURNITURE_H
