#include "project.h"

#include <QFile>


Project::Project() : m_houseSize(HouseSize::Medium) {}

Project::~Project()
{
    clear();
}

bool Project::save(const QString &filename)
{
    QFile file(filename);
    if (!file.open(QIODevice::WriteOnly)) return false;

    QDataStream out(&file);
    out.setVersion(QDataStream::Qt_6_0);

    out << QString("HouseLayoutDesigner");
    out << qint32(1);

    out << qint32(static_cast<int>(m_houseSize));

    out << qint32(m_walls.size());
    for (const Wall &wall : m_walls) {
        out << wall.startPoint() << wall.endPoint();
    }

    out << qint32(m_furniture.size());
    for (const Furniture *item : m_furniture) {
        out << qint32(static_cast<int>(item->type()));
        out << item->position() << item->rotation() << item->isSelected();
    }

    return true;
}

bool Project::load(const QString &filename)
{
    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly)) return false;

    clear();

    QDataStream in(&file);
    in.setVersion(QDataStream::Qt_6_0);

    QString header;
    in >> header;
    if (header != "HouseLayoutDesigner") return false;

    qint32 version;
    in >> version;
    if (version != 1) return false;

    qint32 houseSize;
    in >> houseSize;
    m_houseSize = static_cast<HouseSize>(houseSize);

    qint32 wallCount;
    in >> wallCount;
    for (int i = 0; i < wallCount; ++i) {
        QPoint start, end;
        in >> start >> end;
        m_walls.append(Wall(start, end));
    }

    qint32 furnitureCount;
    in >> furnitureCount;
    for (int i = 0; i < furnitureCount; ++i) {
        qint32 type;
        in >> type;

        QPointF position;
        qreal rotation;
        bool selected;
        in >> position >> rotation >> selected;

        Furniture *item = nullptr;
        switch (static_cast<FurnitureType>(type)) {
        case FurnitureType::Sofa:
            item = new Sofa(position);
            break;
        case FurnitureType::Chair:
            item = new Chair(position);
            break;
        case FurnitureType::Table:
            item = new Table(position);
            break;
        }

        if (item) {
            item->setRotation(rotation);
            item->setSelected(selected);
            m_furniture.append(item);
        }
    }

    return true;
}

void Project::newProject(HouseSize size)
{
    clear();
    m_houseSize = size;
}

QSize Project::getCanvasSize() const
{
    return getSizeFromEnum(m_houseSize);
}

Project::HouseSize Project::getHouseSize() const
{
    return m_houseSize;
}

void Project::setHouseSize(HouseSize size)
{
    m_houseSize = size;
}

QList<Wall> &Project::walls()
{
    return m_walls;
}

const QList<Wall> &Project::walls() const
{
    return m_walls;
}

QList<Furniture *> &Project::furniture()
{
    return m_furniture;
}

const QList<Furniture *> &Project::furniture() const
{
    return m_furniture;
}

void Project::clearFurniture()
{
    for (Furniture *item: m_furniture) {
        delete item;
    }

    m_furniture.clear();
}

void Project::clearWalls()
{
    m_walls.clear();
}

void Project::clear()
{
    clearFurniture();
    clearWalls();
}

QSize Project::getSizeFromEnum(HouseSize size)
{
    switch(size) {
    case HouseSize::Small:
        return QSize(SMALL_WIDTH, SMALL_HEIGHT);
    case HouseSize::Medium:
        return QSize(MEDIUM_WIDTH, MEDIUM_HEIGHT);
    case HouseSize::Large:
        return QSize(LARGE_WIDTH, LARGE_HEIGHT);
    default:
        return QSize(MEDIUM_WIDTH, MEDIUM_HEIGHT);
    }
}
