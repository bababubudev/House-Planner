#ifndef COMMAND_H
#define COMMAND_H

#include "furniture.h"

class Command {
public:
    Command();
    virtual ~Command();

    virtual void execute() = 0;
    virtual void undo() = 0;
    virtual void redo() = 0;
};


class AddFurnitureCommand : public Command {
public:
    AddFurnitureCommand(QList<Furniture*> &furnitureList, Furniture *furniture);
    ~AddFurnitureCommand();

    void execute() override;
    void undo() override;
    void redo() override;

private:
    QList<Furniture*> &m_furnitureList;
    Furniture *m_furniture;
    bool m_ownsItem;
};


class DeleteFurnitureCommand: public Command {
public:
    DeleteFurnitureCommand(QList<Furniture*> &furnitureList, const QList<Furniture*> &selectedFurniture);
    ~DeleteFurnitureCommand();

    void execute() override;
    void undo() override;
    void redo() override;

private:
    QList<Furniture*> &m_furnitureList;
    QList<Furniture*> m_deletedItems;
    QList<int> m_itemIndices;
};


class MoveFurnitureCommand: public Command {
public:
    MoveFurnitureCommand(QList<Furniture*> &furnitureList, const QList<QUuid> &furnitureIds,
                         const QList<QPointF> &oldPoisitions, const QList<QPointF> &newPositions);

    ~MoveFurnitureCommand();

    void execute() override;
    void undo() override;
    void redo() override;

private:
    QList<Furniture*> &m_furnitureList;
    QList<QUuid> m_furnitureIds;
    QList<QPointF> m_oldPositions;
    QList<QPointF> m_newPositions;
};


class RotateFurnitureCommand: public Command {
public:
    RotateFurnitureCommand(Furniture *furniture, qreal oldRotation, qreal newRotation);
    ~RotateFurnitureCommand();

    void execute() override;
    void undo() override;
    void redo() override;

private:
    Furniture *m_furniture;
    qreal m_oldRotation;
    qreal m_newRotation;
};


class AddWallCommand: public Command {
public:
    AddWallCommand(QList<Wall> &wallList, const Wall &wall);
    ~AddWallCommand();

    void execute() override;
    void undo() override;
    void redo() override;

private:
    QList<Wall> &m_wallList;
    Wall m_wall;
};


class DeleteWallCommand: public Command {
public:
    DeleteWallCommand(QList<Wall> &wallList, int wallIndex);
    ~DeleteWallCommand();

    void execute() override;
    void undo() override;
    void redo() override;

private:
    QList<Wall> &m_wallList;
    int m_wallIndex;
    Wall m_deletedWall;
};

class DeleteSelectionCommand: public Command {
public:
    DeleteSelectionCommand(QList<Furniture*> &furnitureList, const QList<Furniture*> &selectedFurniture,
                           QList<Wall> &wallList, const QList<int> &selectedWallIndices);

    ~DeleteSelectionCommand();

    void execute() override;
    void undo() override;
    void redo() override;

private:
    QList<Furniture*> &m_furnitureList;
    QList<Furniture*> m_deletedItems;
    QList<int> m_itemIndices;

    QList<Wall> &m_wallList;
    QList<Wall> m_deletedWalls;
    QList<int> m_wallIndices;
};


#endif // COMMAND_H
