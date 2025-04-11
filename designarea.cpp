#include "designarea.h"

#include <QMessageBox>


DesignArea::DesignArea(QWidget *parent)
    : QWidget(parent), m_toolMode(ToolMode::Select),
    m_isDrawingWall(false), m_isMovingFurniture(false),
    m_isSelecting(false), m_rubberBand(new QRubberBand(QRubberBand::Rectangle, this))
{
    setFocusPolicy(Qt::StrongFocus);
    setMouseTracking(true);

    // Initialize with medium house project
    newProject(Project::HouseSize::Medium);
}

DesignArea::~DesignArea()
{
    delete m_rubberBand;

    for (Furniture *item : m_clipboardFurniture) {
        delete item;
    }
}

void DesignArea::setToolMode(ToolMode mode)
{
    if (m_toolMode != mode) {
        m_toolMode = mode;

        if (m_toolMode == ToolMode::DrawWall) {
            clearSelection();
        }

        setCursor(m_toolMode == ToolMode::DrawWall ? Qt::CrossCursor : Qt::ArrowCursor);
        update();
    }
}

ToolMode DesignArea::getToolMode() const
{
    return m_toolMode;
}

bool DesignArea::canUndo() const
{
    return m_commandManager.canUndo();
}

bool DesignArea::canRedo() const
{
    return m_commandManager.canRedo();
}

QSize DesignArea::sizeHint() const
{
    return m_project.getCanvasSize();
}

QSize DesignArea::minimumSizeHint() const
{
    return m_project.getCanvasSize();
}

void DesignArea::deleteSelection()
{
    if (m_selectedFurniture.isEmpty() && m_selectedWallIndices.isEmpty()) return;

    m_commandManager.execute(new DeleteSelectionCommand(
        m_project.furniture(), m_selectedFurniture,
        m_project.walls(), m_selectedWallIndices
    ));

    m_selectedFurniture.clear();
    m_selectedWallIndices.clear();

    update();
    emit projectModified();
}

void DesignArea::clearSelection()
{
    for (Furniture *item : m_selectedFurniture) {
        item->setSelected(false);
    }

    m_selectedFurniture.clear();
    clearWallSelection();
    update();
}

void DesignArea::selectAll()
{
    clearSelection();
    for (Furniture *item : m_project.furniture()) {
        item->setSelected(true);
        m_selectedFurniture.append(item);
    }

    for (int i = 0; i < m_project.walls().size(); ++i) {
        m_selectedWallIndices.append(i);
    }

    update();
}

void DesignArea::deleteFurniture()
{
    if (m_selectedFurniture.isEmpty()) {
        return;
    }

    m_commandManager.execute(new DeleteFurnitureCommand(m_project.furniture(), m_selectedFurniture));
    m_selectedFurniture.clear();
    update();
}

bool DesignArea::hasSelectedFurniture() const
{
    return !m_selectedFurniture.isEmpty();
}

void DesignArea::copySelectedFurniture()
{
    for (Furniture *item: m_clipboardFurniture) {
        delete item;
    }

    m_clipboardFurniture.clear();

    for (Furniture *item : m_selectedFurniture) {
        m_clipboardFurniture.append(item->clone());
    }
}

void DesignArea::cutSelectedFurniture()
{
    if (m_selectedFurniture.isEmpty()) return;

    copySelectedFurniture();
    deleteFurniture();
}

void DesignArea::pasteFurniture()
{
    if (m_clipboardFurniture.isEmpty()) return;

    clearSelection();

    const int PASTE_OFFSET = 20;
    QPointF offset(PASTE_OFFSET, PASTE_OFFSET);

    for (Furniture *item : m_clipboardFurniture) {
        Furniture *newItem = item->clone();
        newItem->setPosition(newItem->position() + offset);
        newItem->setSelected(true);

        ensureFurnitureInsideCanvas(newItem);

        m_commandManager.execute(new AddFurnitureCommand(m_project.furniture(), newItem));
        m_selectedFurniture.append(newItem);
    }

    update();
}

void DesignArea::rotateFurniture(qreal angle)
{
    if (m_selectedFurniture.size() != 1) {
        return;
    }

    Furniture *item = m_selectedFurniture.first();
    qreal oldRotation = item->rotation();
    qreal newRotation = oldRotation + angle;

    item->setRotation(newRotation);

    if (checkFurnitureCollision(item)) {
        item->setRotation(oldRotation);
        return;
    }

    m_commandManager.execute(new RotateFurnitureCommand(item, oldRotation, newRotation));
    update();
}

void DesignArea::newProject(Project::HouseSize size)
{
    m_project.newProject(size);
    m_commandManager.clear();
    clearSelection();
    setFixedSize(m_project.getCanvasSize());
    update();
}

void DesignArea::saveProject(const QString &filename)
{
    if (!m_project.save(filename)) {
        QMessageBox::warning(this, tr("Save Project"), tr("Failed to save project to %1").arg(filename));
    }
}

void DesignArea::loadProject(const QString &filename)
{
    if (!m_project.load(filename)) {
        QMessageBox::warning(this, tr("Load Project"), tr("Failed to load project from %1").arg(filename));
        return;
    }

    m_commandManager.clear();
    clearSelection();

    for (Furniture *item : m_project.furniture()) {
        item->setSelected(false);
    }

    setFixedSize(m_project.getCanvasSize());
    update();
}

void DesignArea::undo()
{
    m_commandManager.undo();
    clearSelection();

    for (Furniture *item : m_project.furniture()) {
        item->setSelected(false);
    }

    update();
}

void DesignArea::redo()
{
    m_commandManager.redo();
    clearSelection();

    for (Furniture *item : m_project.furniture()) {
        item->setSelected(false);
    }

    update();
}

void DesignArea::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    painter.fillRect(rect(), Qt::white);

    // Draw grid
    painter.setPen(QPen(QColor(230, 230, 230), 1, Qt::SolidLine));
    const int gridSize = 10;
    for (int x = 0; x < width(); x += gridSize) {
        painter.drawLine(x, 0, x, height());
    }
    for (int y = 0; y < height(); y += gridSize) {
        painter.drawLine(0, y, width(), y);
    }


    // Draw walls
    for (int i = 0; i < m_project.walls().size(); ++i) {
        const Wall &wall = m_project.walls()[i];

        painter.setPen(QPen(Qt::black, 5, Qt::SolidLine, Qt::RoundCap));
        wall.draw(painter);

        if (m_selectedWallIndices.contains(i)) {
            painter.save();
            painter.setPen(QPen(Qt::cyan, 5, Qt::SolidLine, Qt::RoundCap));
            wall.draw(painter);

            painter.setPen(QPen(Qt::black, 3, Qt::SolidLine, Qt::RoundCap));
            wall.draw(painter);
            painter.restore();
        }
    }

    // Draw furniture
    for (const Furniture *item: m_project.furniture()) {
        item->draw(painter);
    }

    // Draw wall creation
    if (m_isDrawingWall) {
        painter.save();

        painter.setOpacity(0.5);
        painter.setPen(QPen(Qt::black, 5, Qt::SolidLine, Qt::RoundCap));
        painter.drawLine(m_wallStartPoint, m_wallEndPoint);

        painter.restore();
    }
}

void DesignArea::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        switch (m_toolMode) {
        case ToolMode::Select:
            {
                Furniture *furniture = getFurnitureAt(event->pos());

                if (furniture) {
                    m_isMovingFurniture = true;
                    m_lastMousePos = event->pos();

                    // Clear selection if ctrl is not pressed
                    if (!(event->modifiers() & Qt::ControlModifier) && !furniture->isSelected()) {
                        clearSelection();
                    }

                    if (!furniture->isSelected()) {
                        furniture->setSelected(true);
                        m_selectedFurniture.append(furniture);
                    }

                    m_initialPositions.clear();
                    for (Furniture *item : m_selectedFurniture) {
                        m_initialPositions.append(item->position());
                    }
                }
                else {
                    int wallIndex = getWallAt(event->pos());

                    if (wallIndex >= 0) {
                        if (event->modifiers() & Qt::ControlModifier) {
                            if (m_selectedWallIndices.contains(wallIndex)) {
                                m_selectedWallIndices.removeOne(wallIndex);
                            } else {
                                m_selectedWallIndices.append(wallIndex);
                            }
                        }
                        else {
                            clearSelection();
                            m_selectedWallIndices.clear();
                            m_selectedWallIndices.append(wallIndex);
                        }

                        update();
                    }
                    else {
                        m_isSelecting = true;
                        m_selectionStart = event->pos();
                        m_rubberBand->setGeometry(QRect(m_selectionStart, QSize()));
                        m_rubberBand->show();

                        if (!(event->modifiers() & Qt::ControlModifier)) {
                            clearSelection();
                        }
                    }
                }
            }
            break;
        case ToolMode::DrawWall:
            {
                m_isDrawingWall = true;
                m_wallStartPoint = event->pos();
                m_wallEndPoint = event->pos();
            }
            break;
        case ToolMode::AddChair:
            {
                Furniture *newItem = createFurniture(FurnitureType::Chair, event->pos());

                if (!checkFurnitureCollision(newItem)) {
                    m_commandManager.execute(new AddFurnitureCommand(m_project.furniture(), newItem));
                    emit projectModified();
                    update();
                }
                else {
                    delete newItem;
                }
            }
            break;
        case ToolMode::AddSofa:
            {
                Furniture *newItem = createFurniture(FurnitureType::Sofa, event->pos());

                if (!checkFurnitureCollision(newItem)) {
                    m_commandManager.execute(new AddFurnitureCommand(m_project.furniture(), newItem));
                    emit projectModified();
                    update();
                }
                else {
                    delete newItem;
                }
            }
            break;
        case ToolMode::AddTable:
            {
                Furniture *newItem = createFurniture(FurnitureType::Table, event->pos());

                if (!checkFurnitureCollision(newItem)) {
                    m_commandManager.execute(new AddFurnitureCommand(m_project.furniture(), newItem));
                    emit projectModified();
                    update();
                }
                else {
                    delete newItem;
                }
            }
            break;
        case ToolMode::Rotate:
            {
                if (m_selectedFurniture.isEmpty()){
                    Furniture *furniture = getFurnitureAt(event->pos());
                    if (furniture) {
                        clearSelection();
                        furniture->setSelected(true);
                        m_selectedFurniture.append(furniture);
                        update();
                    }
                }
                else if (m_selectedFurniture.size() == 1) {
                    rotateFurniture(90);
                }
            }
            break;
        }
    }
}

void DesignArea::mouseMoveEvent(QMouseEvent *event)
{
    if (m_isDrawingWall) {
        m_wallEndPoint = event->pos();

        if (event->modifiers() & Qt::ShiftModifier) {
            QPoint diff = m_wallEndPoint - m_wallStartPoint;

            // Horizontal snapping
            if (qAbs(diff.x()) > qAbs(diff.y())) {
                m_wallEndPoint.setY(m_wallStartPoint.y());
            }
            // Vertical snapping
            else {
                m_wallEndPoint.setX(m_wallStartPoint.x());
            }
        }

        update();
    }
    else if (m_isMovingFurniture) {
        QPointF delta = event->pos() - m_lastMousePos;
        m_lastMousePos = event->pos();

        for (int i = 0; i < m_selectedFurniture.size(); ++i) {
            Furniture *item = m_selectedFurniture[i];
            item->setPosition(item->position() + delta);

            ensureFurnitureInsideCanvas(item);
        }

        update();
    }
    else if (m_isSelecting) {
        m_rubberBand->setGeometry(QRect(m_selectionStart, event->pos()).normalized());
    }
}

void DesignArea::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        if (m_isDrawingWall) {
            m_isDrawingWall = false;

            if ((m_wallStartPoint - m_wallEndPoint).manhattanLength() > 10) {
                if (event->modifiers() & Qt::ShiftModifier) {
                    QPoint diff = m_wallEndPoint - m_wallStartPoint;

                    // Horizontal snapping
                    if (qAbs(diff.x()) > qAbs(diff.y())) {
                        m_wallEndPoint.setY(m_wallStartPoint.y());
                    }
                    // Vertical snapping
                    else {
                        m_wallEndPoint.setX(m_wallStartPoint.x());
                    }
                }

                Wall newWall(m_wallStartPoint, m_wallEndPoint);
                m_commandManager.execute(new AddWallCommand(m_project.walls(), newWall));
                emit projectModified();
            }

            update();
        }
        else if (m_isMovingFurniture) {
            m_isMovingFurniture = false;

            QList<QPointF> currentPositions;
            QList<QUuid> furnitureIds;
            bool positionsChanged = false;

            for (int i = 0; i < m_selectedFurniture.size(); ++i) {
                Furniture *item = m_selectedFurniture[i];
                currentPositions.append(item->position());
                furnitureIds.append(item->id());

                if (m_initialPositions[i] != item->position()) {
                    positionsChanged = true;
                }
            }

            if (positionsChanged) {
                bool collisionDetected = false;
                for (Furniture *item : m_selectedFurniture) {
                    if (checkFurnitureCollision(item)) {
                        collisionDetected = true;
                        break;
                    }
                }

                if (collisionDetected) {
                    for (int i = 0; i < m_selectedFurniture.size(); ++i) {
                        m_selectedFurniture[i]->setPosition(m_initialPositions[i]);
                    }
                }
                else {
                    m_commandManager.execute(new MoveFurnitureCommand(m_project.furniture(), furnitureIds, m_initialPositions, currentPositions));
                    emit projectModified();
                }
            }

            m_initialPositions.clear();
            update();
        }
        else if (m_isSelecting) {
            m_isSelecting = false;
            m_rubberBand->hide();

            QRect selectionRect = m_rubberBand->geometry();
            if (selectionRect.width() > 5 && selectionRect.height() > 5) {
                QList<Furniture*> itemsInRect = getFurnitureInRect(selectionRect);
                for (Furniture *item: itemsInRect) {
                    if (!item->isSelected()) {
                        item->setSelected(true);
                        m_selectedFurniture.append(item);
                    }
                }

                for (int i = 0; i < m_project.walls().size(); ++i) {
                    if (isWallInRect(m_project.walls()[i], selectionRect)) {
                        if (!m_selectedWallIndices.contains(i)) {
                            m_selectedWallIndices.append(i);
                        }
                    }
                }
            }

            update();
        }
    }
}

void DesignArea::keyPressEvent(QKeyEvent *event)
{
    switch (event->key()) {
    case Qt::Key_Delete:
        deleteSelection();
        break;
    case Qt::Key_A:
        if (event->modifiers() & Qt::ControlModifier) {
            selectAll();
        }

        break;
    case Qt::Key_C:
        if (event->modifiers() & Qt::ControlModifier) {
            copySelectedFurniture();
        }
        break;
    case Qt::Key_X:
        if (event->modifiers() & Qt::ControlModifier) {
            cutSelectedFurniture();
        }
        break;
    case Qt::Key_V:
        if (event->modifiers() & Qt::ControlModifier) {
            pasteFurniture();
        }
        break;
    case Qt::Key_Z:
        if (event->modifiers() & Qt::ControlModifier) {
            if (event->modifiers() & Qt::ShiftModifier) {
                redo();
            }
            else {
                undo();
            }
        }
        break;
    case Qt::Key_R:
        if (m_selectedFurniture.size() == 1) {
            if (event->modifiers() & Qt::ShiftModifier) {
                rotateFurniture(-90);
            }
            else {
                rotateFurniture(90);
            }
        }
        break;
    case Qt::Key_Escape:
        clearSelection();
        clearWallSelection();
        break;
    default:
        QWidget::keyPressEvent(event);
    }
}

int DesignArea::getWallAt(const QPoint &position)
{
    const int WALL_HIT_DISTANCE = 5;

    for (int i = 0; i < m_project.walls().size(); ++i) {
        const Wall &wall = m_project.walls()[i];
        QLineF line(wall.startPoint(), wall.endPoint());

        qreal distance = calculatePointToLineDistance(position, line);

        if (distance <= WALL_HIT_DISTANCE) {
            return i;
        }
    }

    return -1;
}

void DesignArea::clearWallSelection() {
    m_selectedWallIndices.clear();
    update();
}

void DesignArea::deleteSelectedWall() {
    if (m_selectedWallIndices.isEmpty()) return;

    std::sort(m_selectedWallIndices.begin(), m_selectedWallIndices.end(), std::greater<int>());

    for (int index : m_selectedWallIndices) {
        if (index >= 0 && index < m_project.walls().size()) {
            m_commandManager.execute(new DeleteWallCommand(m_project.walls(), index));
        }
    }

    clearWallSelection();
    update();

    emit projectModified();
}

bool DesignArea::hasSelectedWall() const {
    return !m_selectedWallIndices.isEmpty();
}

int DesignArea::selectedWallsCount() const
{
    return m_selectedWallIndices.size();
}

bool DesignArea::isWallInRect(const Wall &wall, const QRect &rect)
{
    if (rect.contains(wall.startPoint()) && rect.contains(wall.endPoint())) {
        return true;
    }

    QLineF wallLine(wall.startPoint(), wall.endPoint());

    QLineF rectTop(rect.topLeft(), rect.topRight());
    QLineF rectRight(rect.topRight(), rect.bottomRight());
    QLineF rectBottom(rect.bottomRight(), rect.bottomLeft());
    QLineF rectLeft(rect.bottomLeft(), rect.topLeft());

    QPointF intersection;
    return wallLine.intersects(rectTop, &intersection) == QLineF::BoundedIntersection ||
           wallLine.intersects(rectRight, &intersection) == QLineF::BoundedIntersection ||
           wallLine.intersects(rectBottom, &intersection) == QLineF::BoundedIntersection ||
           wallLine.intersects(rectLeft, &intersection) == QLineF::BoundedIntersection;
}

qreal DesignArea::calculatePointToLineDistance(const QPoint &point, const QLineF &line)
{
    qreal numerator = qAbs((line.y2() - line.y1()) * point.x() -
                           (line.x2() - line.x1()) * point.y() +
                           line.x2() * line.y1() - line.y2() * line.x1());
    qreal denominator = qSqrt(qPow(line.y2() - line.y1(), 2) + qPow(line.x2() - line.x1(), 2));

    // Line is a point
    if (denominator == 0) {
        return QLineF(line.p1(), point).length();
    }

    return numerator / denominator;
}

Furniture *DesignArea::createFurniture(FurnitureType type, const QPointF &position)
{
    switch (type) {
    case FurnitureType::Sofa:
        return new Sofa(position);
    case FurnitureType::Chair:
        return new Chair(position);
    case FurnitureType::Table:
        return new Table(position);
    default:
        return nullptr;
    }
}

Furniture *DesignArea::getFurnitureAt(const QPoint &position)
{
    for (int i = m_project.furniture().size() - 1; i >= 0; --i) {
        Furniture *item = m_project.furniture()[i];
        QRectF rect = item->rotatedBoundingRect();

        if (rect.contains(position)) {
            return item;
        }
    }

    return nullptr;
}

QList<Furniture *> DesignArea::getFurnitureInRect(const QRect &rect)
{
    QList<Furniture*> result;
    for (Furniture *item : m_project.furniture()) {
        if (rect.intersects(item->rotatedBoundingRect().toRect())) {
            result.append(item);
        }
    }

    return result;
}

bool DesignArea::checkFurnitureCollision(const Furniture *furniture) const
{
    if (furniture->collidesWith(m_project.walls())) {
        return true;
    }

    for (const Furniture *item : m_project.furniture()) {
        if (furniture != item && furniture->collidesWith(item)) {
            return true;
        }
    }

    return false;
}

void DesignArea::ensureFurnitureInsideCanvas(Furniture *furniture)
{
    QRectF canvasRect(0, 0, width(), height());
    QRectF itemRect = furniture->rotatedBoundingRect();

    QPointF newPos = furniture->position();

    if (itemRect.left() < 0) {
        newPos.setX(newPos.x() - itemRect.left());
    }

    if (itemRect.right() > width()) {
        newPos.setX(newPos.x() - (itemRect.right() - width()));
    }

    if (itemRect.top() < 0) {
        newPos.setY(newPos.y() - itemRect.top());
    }

    if (itemRect.bottom() > height()) {
        newPos.setY(newPos.y() - (itemRect.bottom() - height()));
    }

    furniture->setPosition(newPos);
}
