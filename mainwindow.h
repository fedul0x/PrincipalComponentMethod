#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "mdigraphchild.h"
#include "alglib/linalg.h"
#include "alglib/ap.h"


class MdiChild;
QT_BEGIN_NAMESPACE
class QAction;
class QMenu;
class QMdiArea;
class QMdiSubWindow;
class QSignalMapper;
class QSpinBox;
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow();

protected:
    void closeEvent(QCloseEvent *event);

private slots:
    void newFile();
    void open();
    void save();
    void saveAs();
    void cut();
    void copy();
    void paste();
    void magic();
    void columnValueChange(int i);
    void rowValueChange(int i);

    void about();
    void updateMenus();
    void updateWindowMenu();
    MdiChild *createMdiChild();
    MdiChild *createMdiChild(int n, int m);
    MdiGraphChild *createMdiGraphChild();
    void switchLayoutDirection();
    void setActiveSubWindow(QWidget *window);

private:
    void createActions();
    void createMenus();
    void createToolBars();
    void createStatusBar();
    void readSettings();
    void writeSettings();
    MdiChild *activeMdiChild();
    QMdiSubWindow *findMdiChild(const QString &fileName);

    QMdiArea *mdiArea;
    QSignalMapper *windowMapper;

    QMenu *fileMenu;
    QMenu *editMenu;
    QMenu *windowMenu;
    QMenu *helpMenu;
    QToolBar *fileToolBar;
    QToolBar *editToolBar;
    QAction *newAct;
    QAction *openAct;
    QAction *saveAct;
    QAction *saveAsAct;
    QAction *exitAct;
    QAction *cutAct;
    QAction *copyAct;
    QAction *pasteAct;
    QAction *getCompAct;
    QAction *closeAct;
    QAction *closeAllAct;
    QAction *tileAct;
    QAction *cascadeAct;
    QAction *nextAct;
    QAction *previousAct;
    QAction *separatorAct;
    QAction *aboutAct;
    QAction *aboutQtAct;
    QSpinBox *columnSpinBox;
    QSpinBox *rowSpinBox;
};

#endif
