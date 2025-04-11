
#include "furniture.h"

Furniture::Furniture()
    : m_position(0, 0), m_width(0), m_height(0), m_rotation(0),
    m_type(FurnitureType::Chair), m_selected(false)
{
    m_id = QUuid::createUuid();
}

Furniture::Furniture(const QPointF &position, qreal width, qreal height, FurnitureType type)
    :m_position(position), m_width(width), m_height(height), m_rotation(0),
    m_type(type), m_selected(false)
{
    m_id = QUuid::createUuid();
}

Furniture::~Furniture()
{
}

QPointF Furniture::position() const
{
    return m_position;
}

void Furniture::setPosition(const QPointF &position)
{
    m_position = position;
}

qreal Furniture::width() const
{
    return m_width;
}

qreal Furniture::height() const
{
    return m_height;
}

qreal Furniture::rotation() const
{
    return m_rotation;
}

void Furniture::setRotation(qreal angle)
{
    // 0-360 normalization.
    while (angle < 0) angle += 360;
    while (angle >= 360) angle -= 360;

    m_rotation = angle;
}

FurnitureType Furniture::type() const
{
    return m_type;
}

QString Furniture::typeName() const
{
    switch (m_type) {
    case FurnitureType::Sofa:
        return "Sofa";
    case FurnitureType::Chair:
        return "Chair";
    case FurnitureType::Table:
        return "Table";
    default:
        return "Unknown";
    }
}

QUuid Furniture::id() const
{
    return m_id;
}

QRectF Furniture::boundingRect() const
{
    // x and y positions are centered to the shape
    return QRectF(m_position.x() - m_width / 2, m_position.y() - m_height / 2, m_width, m_height);
}

QRectF Furniture::rotatedBoundingRect() const
{
    if (qFuzzyCompare(m_rotation, 0) || qFuzzyCompare(m_rotation, 360)) {
        return boundingRect();
    }

    QRectF rect = boundingRect();
    QTransform transform;

    transform.translate(m_position.x(), m_position.y());
    transform.rotate(m_rotation);
    transform.translate(-m_position.x(), -m_position.y());

    return transform.mapRect(rect);
}

bool Furniture::isSelected() const
{
    return m_selected;
}

void Furniture::setSelected(bool selected)
{
    m_selected = selected;
}

void Furniture::draw(QPainter &painter) const
{
    painter.save();

    painter.translate(m_position);
    painter.rotate(m_rotation);

    // Centering to 0, 0
    QRectF rect(-m_width / 2, -m_height / 2, m_width, m_height);

    QColor color = getColorForType();
    painter.setBrush(color);

    if (m_selected) {
        painter.setPen(QPen(Qt::blue, 2, Qt::SolidLine));
    }
    else {
        painter.setPen(QPen(Qt::black, 1, Qt::SolidLine));
    }

    painter.drawRect(rect);

    // Direction indicator
    painter.setPen(QPen(Qt::black, 1, Qt::SolidLine));
    painter.drawLine(QPoint(0, 0), QPointF(m_width / 4, 0));

    painter.restore();
}

bool Furniture::collidesWith(const Furniture *other) const
{
    if (other == this) return false;

    return rotatedBoundingRect().intersects(other->rotatedBoundingRect());
}

bool Furniture::collidesWith(const QList<Wall> &walls) const
{
    QRectF rect = rotatedBoundingRect();
    for (const Wall &wall : walls) {
        if (wall.intersects(rect)) return true;
    }

    return false;
}

bool Furniture::collidesWithAny(const QList<Furniture *> &furniture, const QList<Wall> &walls) const
{
    if (collidesWith(walls)) {
        return true;
    }

    for (const Furniture *item : furniture) {
        if (collidesWith(item)) {
            return true;
        }
    }

    return false;
}

QColor Furniture::getColorForType() const
{
    switch (m_type) {
    case FurnitureType::Sofa:
        return QColor(150, 150, 220);
    case FurnitureType::Chair:
        return QColor(200, 150, 150);
    case FurnitureType::Table:
        return QColor(150, 200, 150);
    default:
        return QColor(200, 200, 200);
    }
}

QDataStream &operator<<(QDataStream &stream, const Furniture &furniture) {
    stream << furniture.m_position
           << furniture.m_width
           << furniture.m_height
           << qint32(furniture.m_rotation)
           << qint32(furniture.m_type)
           << furniture.m_id
           << furniture.m_selected;

    return stream;
}

QDataStream &operator>>(QDataStream &stream, Furniture &furniture) {
    qint32 rotation, type;

    stream >> furniture.m_position
        >> furniture.m_width
        >> furniture.m_height
        >> rotation
        >> type
        >> furniture.m_id
        >> furniture.m_selected;

    furniture.m_rotation = rotation;
    furniture.m_type = static_cast<FurnitureType>(type);

    return stream;
}

Sofa::Sofa(): Furniture(QPointF(0, 0), 60, 20, FurnitureType::Sofa) {}

Sofa::Sofa(const QPointF &position): Furniture(QPointF(position), 60, 20, FurnitureType::Sofa) {}

void Sofa::draw(QPainter &painter) const {
    painter.save();

    painter.translate(m_position);
    painter.rotate(m_rotation);

    QRectF rect(-m_width / 2, -m_height / 2, m_width, m_height);
    QRectF backrest(-m_width / 2, -m_height / 2, m_width, m_height / 4);

    QColor color = getColorForType();
    painter.setBrush(color);

    if (m_selected) {
        painter.setPen(QPen(Qt::blue, 2, Qt::SolidLine));
    }
    else {
        painter.setPen(QPen(Qt::black, 1, Qt::SolidLine));
    }

    painter.drawRect(rect);
    painter.setBrush(color.darker(120));
    painter.drawRect(backrest);

    painter.restore();
}

Furniture *Sofa::clone() const {
    Sofa *clone = new Sofa(m_position);
    clone->setRotation(m_rotation);
    clone->setSelected(m_selected);

    return clone;
}

Chair::Chair(): Furniture(QPointF(0, 0), 30, 30, FurnitureType::Chair) {}

Chair::Chair(const QPointF &position): Furniture(QPointF(position), 30, 30, FurnitureType::Chair) {}

void Chair::draw(QPainter &painter) const {
    painter.save();

    painter.translate(m_position);
    painter.rotate(m_rotation);

    QRectF rect(-m_width / 2, -m_height / 2, m_width, m_height);
    QRectF backrest(-m_width / 2, -m_height / 2, m_width / 4, m_height);

    QColor color = getColorForType();
    painter.setBrush(color);

    if (m_selected) {
        painter.setPen(QPen(Qt::blue, 2, Qt::SolidLine));
    }
    else {
        painter.setPen(QPen(Qt::black, 1, Qt::SolidLine));
    }

    painter.drawRect(rect);
    painter.setBrush(color.darker(120));
    painter.drawRect(backrest);

    painter.restore();
}

Furniture *Chair::clone() const {
    Chair *clone = new Chair(m_position);
    clone->setRotation(m_rotation);
    clone->setSelected(m_selected);

    return clone;
}

Table::Table(): Furniture(QPointF(0, 0), 30, 30, FurnitureType::Table) {}

Table::Table(const QPointF &position): Furniture(QPointF(position), 30, 30, FurnitureType::Table) {}

void Table::draw(QPainter &painter) const {
    painter.save();

    painter.translate(m_position);
    painter.rotate(m_rotation);

    qreal radius = qMin(m_width, m_height) / 2;
    QRectF circleRect(-radius, -radius, 2 * radius, 2 * radius);

    QColor color = getColorForType();
    painter.setBrush(color);

    if (m_selected) {
        painter.setPen(QPen(Qt::blue, 2, Qt::SolidLine));
    }
    else {
        painter.setPen(QPen(Qt::black, 1, Qt::SolidLine));
    }

    painter.drawEllipse(circleRect);

    // Cross to represent table
    qreal crossHalfLength = radius * std::sqrt(2) / 2;
    painter.setPen(QPen(Qt::black, 1, Qt::DotLine));
    painter.drawLine(QPointF(-crossHalfLength, -crossHalfLength), QPointF(crossHalfLength, crossHalfLength));
    painter.drawLine(QPointF(-crossHalfLength, crossHalfLength), QPointF(crossHalfLength, -crossHalfLength));

    painter.restore();
}

Furniture *Table::clone() const {
    Table *clone = new Table(m_position);
    clone->setRotation(m_rotation);
    clone->setSelected(m_selected);

    return clone;
}











