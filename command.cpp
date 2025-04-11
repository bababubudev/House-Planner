#include "command.h"


Command::Command() {}

Command::~Command() {}

AddFurnitureCommand::AddFurnitureCommand(QList<Furniture *> &furnitureList, Furniture *furniture)
    : m_furnitureList(furnitureList), m_furniture(furniture) {}

AddFurnitureCommand::~AddFurnitureCommand()
{
    if (m_ownsItem) {
        delete m_furniture;
    }
}

void AddFurnitureCommand::execute()
{
    m_furnitureList.append(m_furniture);
    m_ownsItem = false;
}

void AddFurnitureCommand::undo()
{
    int index = m_furnitureList.indexOf(m_furniture);
    if (index != -1) {
        m_furnitureList.removeAt(index);
        m_ownsItem = true;
    }
}

void AddFurnitureCommand::redo()
{
    execute();
}


DeleteFurnitureCommand::DeleteFurnitureCommand(QList<Furniture *> &furnitureList, const QList<Furniture *> &selectedFurniture)
    : m_furnitureList(furnitureList) {
    for (Furniture *furniture : selectedFurniture) {
        int index = m_furnitureList.indexOf(furniture);
        if (index != -1) {
            m_itemIndices.append(index);
            m_deletedItems.append(furniture->clone());
        }
    }
}

DeleteFurnitureCommand::~DeleteFurnitureCommand()
{
    for (Furniture *item: m_deletedItems) {
        delete item;
    }
}

void DeleteFurnitureCommand::execute()
{
    // Descending order for shifting problem
    std::sort(m_itemIndices.begin(), m_itemIndices.end(), std::greater<int>());

    for (int index: m_itemIndices) {
        if (index < m_furnitureList.size()) {
            delete m_furnitureList.takeAt(index);
        }
    }
}

void DeleteFurnitureCommand::undo()
{
    for (int i = 0; i < m_deletedItems.size(); ++i) {
        int index = m_itemIndices[m_deletedItems.size() - i - 1];
        if (index <= m_furnitureList.size()) {
            m_furnitureList.insert(index, m_deletedItems[i]->clone());
        }
    }
}

void DeleteFurnitureCommand::redo()
{
    execute();
}


MoveFurnitureCommand::MoveFurnitureCommand(QList<Furniture *> &furnitureList, const QList<QUuid> &furnitureIds, const QList<QPointF> &oldPoisitions, const QList<QPointF> &newPositions)
    : m_furnitureList(furnitureList), m_furnitureIds(furnitureIds), m_oldPositions(oldPoisitions), m_newPositions(newPositions) {}

MoveFurnitureCommand::~MoveFurnitureCommand() {}

void MoveFurnitureCommand::execute()
{
    for (int i = 0; i < m_furnitureIds.size(); ++i) {
        for (Furniture *furniture : m_furnitureList) {
            if (furniture->id() == m_furnitureIds[i]) {
                furniture->setPosition(m_newPositions[i]);
                break;
            }
        }
    }
}

void MoveFurnitureCommand::undo()
{
    for (int i = 0; i < m_furnitureIds.size(); ++i) {
        for (Furniture *furniture : m_furnitureList) {
            if (furniture->id() == m_furnitureIds[i]) {
                furniture->setPosition(m_oldPositions[i]);
                break;
            }
        }
    }
}

void MoveFurnitureCommand::redo()
{
    execute();
}


RotateFurnitureCommand::RotateFurnitureCommand(Furniture *furniture, qreal oldRotation, qreal newRotation)
    : m_furniture(furniture), m_oldRotation(oldRotation), m_newRotation(newRotation) {}

RotateFurnitureCommand::~RotateFurnitureCommand() {}

void RotateFurnitureCommand::execute()
{
    m_furniture->setRotation(m_newRotation);
}

void RotateFurnitureCommand::undo()
{
    m_furniture->setRotation(m_oldRotation);
}

void RotateFurnitureCommand::redo()
{
    execute();
}


AddWallCommand::AddWallCommand(QList<Wall> &wallList, const Wall &wall): m_wallList(wallList), m_wall(wall) {}

AddWallCommand::~AddWallCommand() {}

void AddWallCommand::execute()
{
    m_wallList.append(m_wall);
}

void AddWallCommand::undo()
{
    if (!m_wallList.isEmpty()) {
        m_wallList.removeLast();
    }
}

void AddWallCommand::redo()
{
    execute();
}


DeleteWallCommand::DeleteWallCommand(QList<Wall> &wallList, int wallIndex) : m_wallList(wallList), m_wallIndex(wallIndex) {
    if (wallIndex >= 0 && wallIndex < wallList.size()) {
        m_deletedWall = wallList[wallIndex];
    }
}

DeleteWallCommand::~DeleteWallCommand() {}

void DeleteWallCommand::execute()
{
    if (m_wallIndex >= 0 && m_wallIndex < m_wallList.size()) {
        m_wallList.removeAt(m_wallIndex);
    }
}

void DeleteWallCommand::undo()
{
    if (m_wallIndex >= 0 && m_wallIndex < m_wallList.size()) {
        m_wallList.insert(m_wallIndex, m_deletedWall);
    }
}

void DeleteWallCommand::redo()
{
    execute();
}
