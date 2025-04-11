#ifndef DESIGNAREA_H
#define DESIGNAREA_H

#include "commandmanager.h"
#include "project.h"
#include <QKeyEvent>
#include <QMouseEvent>
#include <QPaintEvent>
#include <QRubberBand>
#include <QWidget>

enum class ToolMode {
    Select,
    DrawWall,
    AddSofa,
    AddChair,
    AddTable,
    Rotate
};

class DesignArea: public QWidget {
    Q_OBJECT

public:
    explicit DesignArea(QWidget *parent = nullptr);
    ~DesignArea();

    void setToolMode(ToolMode mode);
    ToolMode getToolMode() const;

    bool canUndo() const;
    bool canRedo() const;

    QSize sizeHint() const override;
    QSize minimumSizeHint() const override;

    void clearSelection();
    void selectAll();
    void deleteFurniture();

    void copySelectedFurniture();
    void cutSelectedFurniture();
    void pasteFurniture();

    void rotateFurniture(qreal angle);

signals:
    void projectModified();

public slots:
    void newProject(Project::HouseSize size);
    void saveProject(const QString &filename);
    void loadProject(const QString &filename);

    void undo();
    void redo();

protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;

private:
    ToolMode m_toolMode;
    Project m_project;

    bool m_isDrawingWall;
    QPoint m_wallStartPoint;
    QPoint m_wallEndPoint;

    bool m_isMovingFurniture;
    QList<Furniture*> m_selectedFurniture;
    QList<QPointF> m_initialPositions;
    QPointF m_lastMousePos;

    bool m_isSelecting;
    QPoint m_selectionStart;
    QRubberBand *m_rubberBand;

    QList<Furniture*> m_clipboardFurniture;

    CommandManager m_commandManager;

    Furniture *createFurniture(FurnitureType type, const QPointF &position);
    Furniture *getFurnitureAt(const QPoint &position);
    QList<Furniture*> getFurnitureInRect(const QRect &rect);
    bool checkFurnitureCollision(const Furniture *furniture) const;
    void ensureFurnitureInsideCanvas(Furniture *furniture);
};

#endif // DESIGNAREA_H
