#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMenu>
#include <QMenuBar>
#include <QAction>
#include <QPixmap>
#include <QProgressBar>

#include "rrtstar_viz.h"

class ConfigObjDialog;
class RRTstar;

class MainWindow : public QMainWindow {
    Q_OBJECT
    
public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();

    RRTstarViz * mpViz;

protected:
    void createMenuBar();
    void createActions();
    bool openMap(QString filename);

    void updateStatus();
private:

    void updateTitle();

    QMenu*   mpFileMenu;
    QAction* mpOpenAction;
    QAction* mpSaveAction;
    QAction* mpExportAction;

    QMenu*   mpEditMenu;
    QAction* mpLoadMapAction;
    QAction* mpLoadObjAction;
    QAction* mpRunAction;

    QMenu*   mpContextMenu;
    QAction* mpAddStartAction;
    QAction* mpAddGoalAction;

    QLabel*       mpStatusLabel;
    QProgressBar* mpStatusProgressBar;

    QPixmap* mpMap;

    QPoint mCursorPoint;

    ConfigObjDialog* mpConfigObjDialog;
    RRTstar*         mpRRTstar;


private slots:
    void contextMenuRequested(QPoint point);
    void onOpen();
    void onSave();
    void onExport();
    void onLoadMap();
    void onLoadObj();
    void onRun();
    void onAddStart();
    void onAddGoal();
};

#endif // MAINWINDOW_H
