#ifndef PROJECT_H
#define PROJECT_H

#include "furniture.h"
#include "wall.h"
#include <QString>


class Project {
public:
    enum class HouseSize {
        Small,
        Medium,
        Large
    };

    Project();
    ~Project();

    bool save(const QString &filename);
    bool load(const QString &filename);

    void newProject(HouseSize size);

    QSize getCanvasSize() const;
    HouseSize getHouseSize() const;
    void setHouseSize(HouseSize size);

    QList<Wall> &walls();
    const QList<Wall> &walls() const;

    QList<Furniture*> &furniture();
    const QList<Furniture*> &furniture() const;

    void clearFurniture();
    void clearWalls();
    void clear();

    static QSize getSizeFromEnum(HouseSize size);

private:
    HouseSize m_houseSize;
    QList<Wall> m_walls;
    QList<Furniture*> m_furniture;

    static const int SMALL_WIDTH = 300;
    static const int SMALL_HEIGHT = 300;

    static const int MEDIUM_WIDTH = 600;
    static const int MEDIUM_HEIGHT = 600;

    static const int LARGE_WIDTH = 800;
    static const int LARGE_HEIGHT = 600;
};

#endif // PROJECT_H
