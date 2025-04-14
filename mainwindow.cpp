#include "mainwindow.h"
#include "./ui_mainwindow.h"

#include <QFileDialog>
#include <QMessageBox>
#include <QToolBar>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , m_currentFile("")
    , m_projectModified(false)
{
    ui->setupUi(this);

    setupDesignArea();
    createActions();
    createMenus();
    createToolbars();
    createStatusBar();

    updateActions();
    updateStatusBar();

    setWindowTitle(tr("House Planner"));
    setMinimumSize(1000, 800);
    this->setIconSize(QSize(18, 18));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::newSmallProject()
{
    m_designArea->newProject(Project::HouseSize::Small);
    m_currentFile = "";
    m_projectModified = false;

    updateStatusBar();
    updateActions();
}

void MainWindow::newMediumProject()
{
    m_designArea->newProject(Project::HouseSize::Medium);
    m_currentFile = "";
    m_projectModified = false;

    updateStatusBar();
    updateActions();
}

void MainWindow::newLargeProject()
{
    m_designArea->newProject(Project::HouseSize::Large);
    m_currentFile = "";
    m_projectModified = false;

    updateStatusBar();
    updateActions();
}

void MainWindow::openProject()
{
    QString filename = QFileDialog::getOpenFileName(this, tr("Open Project"), "", tr("House Planner Files (*.bruh);;All Files (*)"));

    if (filename.isEmpty()) return;

    m_designArea->loadProject(filename);
    m_currentFile = filename;
    m_projectModified = false;

    updateStatusBar();
    updateActions();
}

void MainWindow::saveProject()
{
    if (m_currentFile.isEmpty()) {
        saveProjectAs();
        return;
    }

    m_designArea->saveProject(m_currentFile);
    m_projectModified = false;

    updateStatusBar();
}

void MainWindow::saveProjectAs()
{
    QString filename = QFileDialog::getSaveFileName(this, tr("Save Project"), "", tr("House Layout Files (*.bruh);;All Files (*)"));

    if (filename.isEmpty()) return;

    m_designArea->saveProject(filename);
    m_currentFile = filename;
    m_projectModified = false;

    updateStatusBar();
}

void MainWindow::undo()
{
    m_designArea->undo();
    m_projectModified = true;

    updateStatusBar();
    updateActions();
}

void MainWindow::redo()
{
    m_designArea->redo();
    m_projectModified = true;

    updateStatusBar();
    updateActions();
}

void MainWindow::cutSelectedFurniture()
{
    m_designArea->cutSelectedFurniture();
    m_projectModified = true;

    updateStatusBar();
    updateActions();
}

void MainWindow::copySelectedFurniture()
{
    m_designArea->copySelectedFurniture();
    updateActions();
}

void MainWindow::pasteFurniture()
{
    m_designArea->pasteFurniture();
    m_projectModified = true;

    updateStatusBar();
    updateActions();
}

void MainWindow::deleteSelection()
{
    m_designArea->deleteSelection();
    m_projectModified = true;

    updateStatusBar();
    updateActions();
}

void MainWindow::selectAll()
{
    m_designArea->selectAll();
    updateActions();
}

void MainWindow::rotateFurnitureClockwise()
{
    m_designArea->rotateFurniture(45);
    m_projectModified = true;

    updateStatusBar();
}

void MainWindow::rotateFurnitureAntiClockwise()
{
    m_designArea->rotateFurniture(-45);
    m_projectModified = true;

    updateStatusBar();
}

void MainWindow::setSelectMode()
{
    m_designArea->setToolMode(ToolMode::Select);
    updateActions();
}

void MainWindow::setWallMode()
{
    m_designArea->setToolMode(ToolMode::DrawWall);
    updateActions();
}

void MainWindow::setSofaMode()
{
    m_designArea->setToolMode(ToolMode::AddSofa);
    updateActions();
}

void MainWindow::setChairMode()
{
    m_designArea->setToolMode(ToolMode::AddChair);
    updateActions();
}

void MainWindow::setTableMode()
{
    m_designArea->setToolMode(ToolMode::AddTable);
    updateActions();
}

void MainWindow::setRotateMode()
{
    m_designArea->setToolMode(ToolMode::Rotate);
    updateActions();
}

void MainWindow::updateStatusBar()
{
    QString status;

    if (m_currentFile.isEmpty()) {
        status = tr("  Unnamed Project");
    }
    else {
        QFileInfo fileInfo(m_currentFile);
        status = fileInfo.fileName();
    }

    if (m_projectModified) {
        status += tr(" [modified]");
    }

    QFont font;
    font.setFamily("Helvetica");
    font.setPixelSize(11);
    font.setWeight(QFont::Light);

    m_statusLabel->setFont(font);
    m_statusLabel->setText(status);
}

void MainWindow::updateActions()
{
    m_undoAction->setEnabled(m_designArea->canUndo());
    m_redoAction->setEnabled(m_designArea->canRedo());

    ToolMode mode = m_designArea->getToolMode();
    m_selectAction->setChecked(mode == ToolMode::Select);
    m_wallAction->setChecked(mode == ToolMode::DrawWall);
    m_sofaAction->setChecked(mode == ToolMode::AddSofa);
    m_chairAction->setChecked(mode == ToolMode::AddChair);
    m_tableAction->setChecked(mode == ToolMode::AddTable);
    m_rotateAction->setChecked(mode == ToolMode::Rotate);
}

void MainWindow::setProjectModified()
{
    m_projectModified = true;
    updateStatusBar();
}


QIcon tintIcon(const QString &iconPath, const QColor &color) {
    QPixmap originalPixmap(iconPath);
    if (originalPixmap.isNull())
        return QIcon(iconPath);  // Return original if loading fails

    // Create a same-sized pixmap for the colorized version
    QPixmap coloredPixmap(originalPixmap.size());
    coloredPixmap.fill(Qt::transparent); // Start with transparency

    // Create painter for the new pixmap
    QPainter painter(&coloredPixmap);

    // Draw the original icon
    painter.drawPixmap(0, 0, originalPixmap);

    // Apply color overlay while preserving transparency
    painter.setCompositionMode(QPainter::CompositionMode_SourceIn);
    painter.fillRect(coloredPixmap.rect(), color);
    painter.end();

    return QIcon(coloredPixmap);
}

void MainWindow::createActions()
{
    m_newSmallAction = new QAction(tr("New Small (300x300)"), this);
    m_newSmallAction->setShortcut(tr("Ctrl+Shift+S"));
    connect(m_newSmallAction, &QAction::triggered, this, &MainWindow::newSmallProject);

    m_newMediumAction = new QAction(tr("New Medium (600x600)"), this);
    m_newMediumAction->setShortcut(tr("Ctrl+Shift+M"));
    connect(m_newMediumAction, &QAction::triggered, this, &MainWindow::newMediumProject);

    m_newLargeAction = new QAction(tr("New Large (800x600)"), this);
    m_newLargeAction->setShortcut(tr("Ctrl+Shift+L"));
    connect(m_newLargeAction, &QAction::triggered, this, &MainWindow::newLargeProject);

    m_openAction = new QAction(tr("&Open..."), this);
    m_openAction->setShortcut(tr("Ctrl+O"));
    connect(m_openAction, &QAction::triggered, this, &MainWindow::openProject);

    m_saveAction = new QAction(tr("&Save..."), this);
    m_saveAction->setShortcut(tr("Ctrl+S"));
    connect(m_saveAction, &QAction::triggered, this, &MainWindow::saveProject);

    m_saveAsAction = new QAction(tr("Save &As..."), this);
    connect(m_saveAsAction, &QAction::triggered, this, &MainWindow::saveProjectAs);

    m_exitAction = new QAction(tr("E&xit"), this);
    m_exitAction->setShortcut(tr("Ctrl+Q"));
    connect(m_exitAction, &QAction::triggered, this, &MainWindow::close);

    m_undoAction = new QAction(tr("&Undo"), this);
    m_undoAction->setShortcut(tr("Ctrl+Z"));
    connect(m_undoAction, &QAction::triggered, this, &MainWindow::undo);

    m_redoAction = new QAction(tr("&Redo"), this);
    m_redoAction->setShortcut(tr("Ctrl+Y"));
    connect(m_redoAction, &QAction::triggered, this, &MainWindow::redo);

    m_cutAction = new QAction(tr("Cu&t"), this);
    m_cutAction->setShortcut(tr("Ctrl+X"));
    connect(m_cutAction, &QAction::triggered, this, &MainWindow::cutSelectedFurniture);

    m_copyAction = new QAction(tr("&Copy"), this);
    m_copyAction->setShortcut(tr("Ctrl+C"));
    connect(m_copyAction, &QAction::triggered, this, &MainWindow::copySelectedFurniture);

    m_pasteAction = new QAction(tr("&Paste"), this);
    m_pasteAction->setShortcut(tr("Ctrl+V"));
    connect(m_pasteAction, &QAction::triggered, this, &MainWindow::pasteFurniture);

    m_deleteAction = new QAction(tr("&Delete"), this);
    m_deleteAction->setShortcut(tr("Delete"));
    connect(m_deleteAction, &QAction::triggered, this, &MainWindow::deleteSelection);


    m_selectAllAction = new QAction(tr("Select &All"), this);
    m_selectAllAction->setShortcut(tr("Ctrl+A"));
    connect(m_selectAllAction, &QAction::triggered, this, &MainWindow::selectAll);

    m_rotateClockwiseAction = new QAction(tr("Rotate &Clockwise"), this);
    m_rotateClockwiseAction->setShortcut(tr("R"));
    connect(m_rotateClockwiseAction, &QAction::triggered, this, &MainWindow::rotateFurnitureClockwise);

    m_rotateAntiClockwiseAction = new QAction(tr("Rotate &Anti-clockwise"), this);
    m_rotateAntiClockwiseAction->setShortcut(tr("Shift+R"));
    connect(m_rotateAntiClockwiseAction, &QAction::triggered, this, &MainWindow::rotateFurnitureAntiClockwise);

    m_selectAction = new QAction(tr("Select"), this);
    m_selectAction->setShortcut(tr("A"));
    m_selectAction->setCheckable(true);
    m_selectAction->setChecked(true);
    connect(m_selectAction, &QAction::triggered, this, &MainWindow::setSelectMode);

    m_wallAction = new QAction(tr("Draw Wall"), this);
    m_wallAction->setShortcut(tr("W"));
    m_wallAction->setCheckable(true);
    connect(m_wallAction, &QAction::triggered, this, &MainWindow::setWallMode);

    m_sofaAction = new QAction(tr("Add Sofa"), this);
    m_sofaAction->setShortcut(tr("S"));
    m_sofaAction->setCheckable(true);
    connect(m_sofaAction, &QAction::triggered, this, &MainWindow::setSofaMode);

    m_chairAction = new QAction(tr("Add Chair"), this);
    m_chairAction->setShortcut(tr("C"));
    m_chairAction->setCheckable(true);
    connect(m_chairAction, &QAction::triggered, this, &MainWindow::setChairMode);

    m_tableAction = new QAction(tr("Add Table"), this);
    m_tableAction->setShortcut(tr("T"));
    m_tableAction->setCheckable(true);
    connect(m_tableAction, &QAction::triggered, this, &MainWindow::setTableMode);

    m_rotateAction = new QAction(tr("Rotate"), this);
    m_rotateAction->setCheckable(true);
    connect(m_rotateAction, &QAction::triggered, this, &MainWindow::setRotateMode);

    m_newSmallAction->setIcon(tintIcon(":/resource/icons/new.png", QColor(225, 225, 225)));
    m_newMediumAction->setIcon(tintIcon(":/resource/icons/new.png", QColor(225, 225, 225)));
    m_newLargeAction->setIcon(tintIcon(":/resource/icons/new.png", QColor(225, 225, 225)));
    m_openAction->setIcon(tintIcon(":/resource/icons/open.png", QColor(225, 225, 225)));
    m_saveAction->setIcon(tintIcon(":/resource/icons/save.png", QColor(225, 225, 225)));
    m_undoAction->setIcon(tintIcon(":/resource/icons/undo.png", QColor(225, 225, 225)));
    m_redoAction->setIcon(tintIcon(":/resource/icons/redo.png", QColor(225, 225, 225)));
    m_cutAction->setIcon(tintIcon(":/resource/icons/cut.png", QColor(225, 225, 225)));
    m_copyAction->setIcon(tintIcon(":/resource/icons/copy.png", QColor(225, 225, 225)));
    m_pasteAction->setIcon(tintIcon(":/resource/icons/paste.png", QColor(225, 225, 225)));
    m_deleteAction->setIcon(tintIcon(":/resource/icons/delete.png", QColor(225, 225, 225)));
    m_selectAction->setIcon(tintIcon(":/resource/icons/select.png", QColor(225, 225, 225)));
    m_wallAction->setIcon(tintIcon(":/resource/icons/wall.png", QColor(225, 225, 225)));
    m_sofaAction->setIcon(tintIcon(":/resource/icons/sofa.png", QColor(225, 225, 225)));
    m_chairAction->setIcon(tintIcon(":/resource/icons/chair.png", QColor(225, 225, 225)));
    m_tableAction->setIcon(tintIcon(":/resource/icons/table.png", QColor(225, 225, 225)));
    m_rotateAction->setIcon(tintIcon(":/resource/icons/rotate.png", QColor(225, 225, 225)));
    m_rotateClockwiseAction->setIcon(tintIcon(":/resource/icons/rotate.png", QColor(225, 225, 225)));
    m_rotateAntiClockwiseAction->setIcon(tintIcon(":/resource/icons/rotate.png", QColor(225, 225, 225)));
}

void MainWindow::createMenus()
{
    QMenu *fileMenu = menuBar()->addMenu(tr("&File"));
    QMenu *newMenu = fileMenu->addMenu(tr("New"));
    newMenu->addAction(m_newSmallAction);
    newMenu->addAction(m_newMediumAction);
    newMenu->addAction(m_newLargeAction);

    fileMenu->addAction(m_openAction);
    fileMenu->addAction(m_saveAction);
    fileMenu->addAction(m_saveAsAction);
    fileMenu->addSeparator();
    fileMenu->addAction(m_exitAction);

    QMenu *editMenu = menuBar()->addMenu(tr("&Edit"));
    editMenu->addAction(m_undoAction);
    editMenu->addAction(m_redoAction);
    editMenu->addSeparator();
    editMenu->addAction(m_cutAction);
    editMenu->addAction(m_copyAction);
    editMenu->addAction(m_pasteAction);
    editMenu->addAction(m_deleteAction);
    editMenu->addSeparator();
    editMenu->addAction(m_selectAllAction);
    editMenu->addSeparator();
    editMenu->addAction(m_rotateClockwiseAction);
    editMenu->addAction(m_rotateAntiClockwiseAction);

    QMenu *toolsMenu = menuBar()->addMenu(tr("&Tools"));
    toolsMenu->addAction(m_selectAction);
    toolsMenu->addAction(m_wallAction);
    toolsMenu->addAction(m_sofaAction);
    toolsMenu->addAction(m_chairAction);
    toolsMenu->addAction(m_tableAction);
    toolsMenu->addAction(m_rotateAction);
}

void MainWindow::createToolbars()
{
    QToolBar *fileToolBar = addToolBar(tr("File"));
    fileToolBar->addAction(m_newMediumAction);
    fileToolBar->addAction(m_openAction);
    fileToolBar->addAction(m_saveAction);

    QToolBar *editToolBar = addToolBar(tr("Edit"));
    editToolBar->addAction(m_undoAction);
    editToolBar->addAction(m_redoAction);
    editToolBar->addSeparator();
    editToolBar->addAction(m_cutAction);
    editToolBar->addAction(m_copyAction);
    editToolBar->addAction(m_pasteAction);
    editToolBar->addAction(m_deleteAction);

    QToolBar *toolsToolBar = addToolBar(tr("Tools"));
    toolsToolBar->addAction(m_selectAction);
    toolsToolBar->addAction(m_rotateAction);
    toolsToolBar->addAction(m_wallAction);
    toolsToolBar->addAction(m_sofaAction);
    toolsToolBar->addAction(m_chairAction);
    toolsToolBar->addAction(m_tableAction);

    toolsToolBar->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    toolsToolBar->setStyleSheet("QToolButton::handle { qproperty-iconColor: white; } ");
}

void MainWindow::createStatusBar()
{
    m_statusLabel = new QLabel(tr("New Project"));
    statusBar()->addWidget(m_statusLabel);
}

void MainWindow::setupDesignArea()
{
    m_designArea = new DesignArea(this);
    m_scrollArea = new QScrollArea;
    m_scrollArea->setWidget(m_designArea);
    m_scrollArea->setWidgetResizable(false);
    m_scrollArea->setAlignment(Qt::AlignCenter);
    setCentralWidget(m_scrollArea);

    connect(m_designArea->findChild<CommandManager*>(), &CommandManager::undoRedoStateChanged, this, &MainWindow::updateActions);
    connect(m_designArea, &DesignArea::projectModified, this, &MainWindow::setProjectModified);
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    if (m_projectModified) {
        QMessageBox::StandardButton ret = QMessageBox::warning(this, tr("House Layout Designer"),
                                                               tr("The project has been modified.\nDo you want to save your changes?"),
                                                               QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);

        if (ret == QMessageBox::Save) {
            saveProject();
            event->accept();
        }
        else if (ret == QMessageBox::Cancel) {
            event->ignore();
        }
        else {
            event->accept();
        }
    } else {
        event->accept();
    }
}
