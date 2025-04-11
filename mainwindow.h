#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "designarea.h"
#include <QCloseEvent>
#include <QLabel>
#include <QAction>
#include <QMainWindow>
#include <QScrollArea>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void newSmallProject();
    void newMediumProject();
    void newLargeProject();
    void openProject();
    void saveProject();
    void saveProjectAs();

    void undo();
    void redo();

    void cutSelectedFurniture();
    void copySelectedFurniture();
    void pasteFurniture();
    void deleteSelection();
    void selectAll();
    void rotateFurnitureClockwise();
    void rotateFurnitureAntiClockwise();

    void setSelectMode();
    void setWallMode();
    void setSofaMode();
    void setChairMode();
    void setTableMode();
    void setRotateMode();

    void updateStatusBar();
    void updateActions();
    void setProjectModified();

private:
    void createActions();
    void createMenus();
    void createToolbars();
    void createStatusBar();

    void setupDesignArea();

    void closeEvent(QCloseEvent *event) override;

    Ui::MainWindow *ui;

    DesignArea *m_designArea;
    QScrollArea *m_scrollArea;

    QAction *m_newSmallAction;
    QAction *m_newMediumAction;
    QAction *m_newLargeAction;
    QAction *m_openAction;
    QAction *m_saveAction;
    QAction *m_saveAsAction;
    QAction *m_exitAction;

    QAction *m_undoAction;
    QAction *m_redoAction;
    QAction *m_cutAction;
    QAction *m_copyAction;
    QAction *m_pasteAction;
    QAction *m_deleteAction;
    QAction *m_selectAllAction;
    QAction *m_rotateClockwiseAction;
    QAction *m_rotateAntiClockwiseAction;

    QAction *m_selectAction;
    QAction *m_wallAction;
    QAction *m_sofaAction;
    QAction *m_chairAction;
    QAction *m_tableAction;
    QAction *m_rotateAction;

    QLabel *m_statusLabel;

    QString m_currentFile;
    bool m_projectModified;
};
#endif // MAINWINDOW_H
