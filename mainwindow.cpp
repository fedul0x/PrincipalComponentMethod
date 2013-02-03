#include <QtGui>

#include "mainwindow.h"
#include "mdichild.h"
#include <math.h>

MainWindow::MainWindow()
{
    mdiArea = new QMdiArea;
    mdiArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    mdiArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    setCentralWidget(mdiArea);
    connect(mdiArea, SIGNAL(subWindowActivated(QMdiSubWindow*)),
            this, SLOT(updateMenus()));
    windowMapper = new QSignalMapper(this);
    connect(windowMapper, SIGNAL(mapped(QWidget*)),
            this, SLOT(setActiveSubWindow(QWidget*)));

    createActions();
    createMenus();
    createToolBars();
    createStatusBar();
    updateMenus();

    readSettings();

    setWindowTitle(tr("Yuri Scherbakov - Svd Program"));
    setUnifiedTitleAndToolBarOnMac(true);
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    mdiArea->closeAllSubWindows();
    if (mdiArea->currentSubWindow()) {
        event->ignore();
    } else {
        writeSettings();
        event->accept();
    }
}

void MainWindow::newFile()
{
    MdiChild *child = createMdiChild();
    child->newFile();
    child->show();
}

void MainWindow::open()
{
    QString fileName = QFileDialog::getOpenFileName(this);
    if (!fileName.isEmpty()) {
        QMdiSubWindow *existing = findMdiChild(fileName);
        if (existing) {
            mdiArea->setActiveSubWindow(existing);
            return;
        }

        MdiChild *child = createMdiChild();
        if (child->loadFile(fileName)) {
            statusBar()->showMessage(tr("File loaded"), 2000);
            child->show();
        } else {
            child->close();
        }
    }
}

void MainWindow::save()
{
    if (activeMdiChild() && activeMdiChild()->save())
        statusBar()->showMessage(tr("File saved"), 2000);
}

void MainWindow::saveAs()
{
    if (activeMdiChild() && activeMdiChild()->saveAs())
        statusBar()->showMessage(tr("File saved"), 2000);
}

void MainWindow::cut()
{
    //    if (activeMdiChild())
    //        activeMdiChild()->cut();
}

void MainWindow::copy()
{
    if (activeMdiChild())
        activeMdiChild()->copy();
}

void MainWindow::paste()
{
    if (activeMdiChild())
        activeMdiChild()->paste();
}

void MainWindow::columnValueChange(int i) {
    if (activeMdiChild())
        activeMdiChild()->setColumnCount(i);
}

void MainWindow::rowValueChange(int i) {
    if (activeMdiChild())
        activeMdiChild()->setRowCount(i);
}

void MainWindow::magic()
{
    if (!activeMdiChild())
        return;

    MdiChild * tmp = activeMdiChild();
    int n = tmp->columnCount();
    int m = tmp->rowCount();
    MdiChild *child = createMdiChild(n, m);
    child->newFile();
    child->setColumnsTitlesPrefix(tr("Score %1"));
    child->setRowsTitlesPrefix(tr("Expert %1"));
    child->setCurFile(tr("Autoscales matrix"));
    QString str;
    float element, sum, sigma;
    bool ok = false;
    alglib::real_2d_array a;
    a.setlength(m, n);

    child->show();
    for(int j=0; j<n; j++) {
        sum = 0.0;
        for(int i=0; i<m; i++) {
            if (tmp->item(i,j))
                str = tmp->item(i,j)->text();
            else
                str = "0";
            element = str.toFloat(&ok);
            sum += element;
        }
        sum /= m;
        sigma = 0.0;
        for(int i=0; i<m; i++) {
            if (tmp->item(i,j))
                str = tmp->item(i,j)->text();
            else
                str = "0";
            element = str.toFloat(&ok);
            sigma += qPow(element - sum, 2);
        }
        sigma = sqrt(sigma/(m-1));
        for(int i1=0; i1<m; i1++) {
            if (tmp->item(i1,j))
                str = tmp->item(i1,j)->text();
            else
                str = "0";
            element = str.toFloat(&ok);
            if (sigma)
                a[i1][j] = (element-sum)/sigma;
            else
                a[i1][j] = 0;
            QTableWidgetItem *newItem = new QTableWidgetItem(tr("%1").arg(a[i1][j], 0, 'g', 4));
            child->setItem(i1, j, newItem);
        }
    }
    alglib::real_1d_array w;
    alglib::real_2d_array u;
    alglib::real_2d_array vt;
    bool isSolved = alglib::rmatrixsvd(a, m, n,
                                       1, 1, 0,
                                       w, u, vt);
    child->show();
    if (!isSolved) return;
    MdiChild *childW = createMdiChild(n, m);
    childW->newFile();
    childW->setCurFile(tr("W -  vector of singular values"));
    childW->setColumnsTitlesPrefix("%1");
    childW->setRowsTitlesPrefix("%1");
    childW->setRowCount(w.length());
    childW->setColumnCount(1);
    MdiChild *childU = createMdiChild(n, m);
    childU->newFile();
    childU->setCurFile(tr("U - matrix from A=U*S*V^T"));
    childU->setColumnsTitlesPrefix("%1");
    childU->setRowsTitlesPrefix("%1");
    childU->setRowCount(u.rows());
    childU->setColumnCount(u.cols());
    MdiChild *childVt = createMdiChild(n, m);
    childVt->newFile();
    childVt->setCurFile(tr("V - transpose load matrix"));
    childVt->setColumnsTitlesPrefix("%1");
    childVt->setRowsTitlesPrefix("%1");
    childVt->setRowCount(vt.rows());
    childVt->setColumnCount(vt.cols());
    QTableWidgetItem *newItem;
    for(int i=0; i<w.length(); i++) {
        newItem = new QTableWidgetItem(tr("%1").arg(w[i], 0, 'g', 4));
        childW->setItem(i, 0, newItem);
    }
    for(int i=0; i<u.rows(); i++) {
        for(int j=0; j<u.cols(); j++) {
            newItem = new QTableWidgetItem(tr("%1").arg(u[i][j], 0, 'g', 4));
            childU->setItem(i, j, newItem);
        }
    }
    for(int i=0; i<vt.rows(); i++) {
        for(int j=0; j<vt.cols(); j++) {
            newItem = new QTableWidgetItem(tr("%1").arg(vt[i][j], 0, 'g', 4));
            childVt->setItem(i, j, newItem);
        }
    }
    childW->show();;
    childU->show();
    childVt->show();
    alglib::real_2d_array d;
    alglib::real_2d_array t;
    d.setlength(m, n);
    t.setlength(m, n);
    for(int i=0; i<w.length(); i++)
        d[i][i] = w[i];
    alglib::rmatrixgemm(u.rows(), d.cols(), u.cols(), 1,
                        u, 0, 0, 0,
                        d, 0, 0, 0, 0.0,
                        t, 0, 0);
    MdiChild *childT = createMdiChild(n, m);
    childT->newFile();
    childT->setCurFile(tr("T - account matix"));
    childT->setColumnsTitlesPrefix(tr("Component %1"));
    childT->setRowsTitlesPrefix("%1");
    childT->setRowCount(t.rows());
    childT->setColumnCount(t.cols());
    for(int i=0; i<t.rows(); i++) {
        for(int j=0; j<t.cols(); j++) {
            newItem = new QTableWidgetItem(tr("%1").arg(t[i][j], 0, 'g', 4));
            childT->setItem(i, j, newItem);
        }
    }
    childT->show();
    mdiArea->tileSubWindows();
}

void MainWindow::about()
{
    QMessageBox::about(this, tr("About this program"),
                       tr("The <b>svd</b> algorithm program for Yuri Scherbakov diploma work."));
}

void MainWindow::updateMenus()
{
    bool hasMdiChild = (activeMdiChild() != 0);
    saveAct->setEnabled(hasMdiChild);
    saveAsAct->setEnabled(hasMdiChild);
    pasteAct->setEnabled(hasMdiChild);
    closeAct->setEnabled(hasMdiChild);
    closeAllAct->setEnabled(hasMdiChild);
    tileAct->setEnabled(hasMdiChild);
    cascadeAct->setEnabled(hasMdiChild);
    nextAct->setEnabled(hasMdiChild);
    previousAct->setEnabled(hasMdiChild);
    separatorAct->setVisible(hasMdiChild);

    bool hasSelection = true;
    //    bool hasSelection = (activeMdiChild() &&
    //                         activeMdiChild()->textCursor().hasSelection());
    cutAct->setEnabled(hasSelection);
    copyAct->setEnabled(hasSelection);
    //    getCompAct->setEnabled(hasMdiChild && !(activeMdiChild()->isReadOnly()));
    columnSpinBox->setEnabled(hasMdiChild);
    rowSpinBox->setEnabled(hasMdiChild);
    if (hasMdiChild && (activeMdiChild()->columnCount()) && (activeMdiChild()->rowCount())){
        columnSpinBox->setValue(activeMdiChild()->columnCount());
        rowSpinBox->setValue(activeMdiChild()->rowCount());
    }


}

void MainWindow::updateWindowMenu()
{
    windowMenu->clear();
    windowMenu->addAction(closeAct);
    windowMenu->addAction(closeAllAct);
    windowMenu->addSeparator();
    windowMenu->addAction(tileAct);
    windowMenu->addAction(cascadeAct);
    windowMenu->addSeparator();
    windowMenu->addAction(nextAct);
    windowMenu->addAction(previousAct);
    windowMenu->addAction(separatorAct);

    QList<QMdiSubWindow *> windows = mdiArea->subWindowList();
    separatorAct->setVisible(!windows.isEmpty());

    for (int i = 0; i < windows.size(); ++i) {
        MdiChild *child = qobject_cast<MdiChild *>(windows.at(i)->widget());

        QString text;
        if (i < 9) {
            text = tr("&%1 %2").arg(i + 1)
                    .arg(child->userFriendlyCurrentFile());
        } else {
            text = tr("%1 %2").arg(i + 1)
                    .arg(child->userFriendlyCurrentFile());
        }
        QAction *action  = windowMenu->addAction(text);
        action->setCheckable(true);
        action ->setChecked(child == activeMdiChild());
        connect(action, SIGNAL(triggered()), windowMapper, SLOT(map()));
        windowMapper->setMapping(action, windows.at(i));
    }
}

MdiChild *MainWindow::createMdiChild()
{
    MdiChild *child = new MdiChild;
    mdiArea->addSubWindow(child);

    connect(child, SIGNAL(copyAvailable(bool)),
            cutAct, SLOT(setEnabled(bool)));
    connect(child, SIGNAL(copyAvailable(bool)),
            copyAct, SLOT(setEnabled(bool)));

    return child;
}

MdiChild *MainWindow::createMdiChild(int n, int m)
{
    MdiChild *child = new MdiChild;
    mdiArea->addSubWindow(child);
    child->setColumnCount(n);
    child->setRowCount(m);

    connect(child, SIGNAL(copyAvailable(bool)),
            cutAct, SLOT(setEnabled(bool)));
    connect(child, SIGNAL(copyAvailable(bool)),
            copyAct, SLOT(setEnabled(bool)));

    return child;
}

void MainWindow::createActions()
{
    newAct = new QAction(QIcon(":/images/new.png"), tr("&New"), this);
    newAct->setShortcuts(QKeySequence::New);
    newAct->setStatusTip(tr("Create a new file"));
    connect(newAct, SIGNAL(triggered()), this, SLOT(newFile()));

    openAct = new QAction(QIcon(":/images/open.png"), tr("&Open..."), this);
    openAct->setShortcuts(QKeySequence::Open);
    openAct->setStatusTip(tr("Open an existing file"));
    connect(openAct, SIGNAL(triggered()), this, SLOT(open()));

    saveAct = new QAction(QIcon(":/images/save.png"), tr("&Save"), this);
    saveAct->setShortcuts(QKeySequence::Save);
    saveAct->setStatusTip(tr("Save the document to disk"));
    connect(saveAct, SIGNAL(triggered()), this, SLOT(save()));

    saveAsAct = new QAction(tr("Save &As..."), this);
    saveAsAct->setShortcuts(QKeySequence::SaveAs);
    saveAsAct->setStatusTip(tr("Save the document under a new name"));
    connect(saveAsAct, SIGNAL(triggered()), this, SLOT(saveAs()));

    //! [0]
    exitAct = new QAction(tr("E&xit"), this);
    exitAct->setShortcuts(QKeySequence::Quit);
    exitAct->setStatusTip(tr("Exit the application"));
    connect(exitAct, SIGNAL(triggered()), qApp, SLOT(closeAllWindows()));
    //! [0]

    cutAct = new QAction(QIcon(":/images/cut.png"), tr("Cu&t"), this);
    cutAct->setShortcuts(QKeySequence::Cut);
    cutAct->setStatusTip(tr("Cut the current selection's contents to the "
                            "clipboard"));
    connect(cutAct, SIGNAL(triggered()), this, SLOT(cut()));

    copyAct = new QAction(QIcon(":/images/copy.png"), tr("&Copy"), this);
    copyAct->setShortcuts(QKeySequence::Copy);
    copyAct->setStatusTip(tr("Copy the current selection's contents to the "
                             "clipboard"));
    connect(copyAct, SIGNAL(triggered()), this, SLOT(copy()));

    pasteAct = new QAction(QIcon(":/images/paste.png"), tr("&Paste"), this);
    pasteAct->setShortcuts(QKeySequence::Paste);
    pasteAct->setStatusTip(tr("Paste the clipboard's contents into the current "
                              "selection"));
    connect(pasteAct, SIGNAL(triggered()), this, SLOT(paste()));

    getCompAct = new QAction(QIcon(":/images/stick.png"), tr("&Make"), this);
    getCompAct->setStatusTip(tr("Make magic"));
    connect(getCompAct, SIGNAL(triggered()), this, SLOT(magic()));

    closeAct = new QAction(tr("Cl&ose"), this);
    closeAct->setStatusTip(tr("Close the active window"));
    connect(closeAct, SIGNAL(triggered()),
            mdiArea, SLOT(closeActiveSubWindow()));

    closeAllAct = new QAction(tr("Close &All"), this);
    closeAllAct->setStatusTip(tr("Close all the windows"));
    connect(closeAllAct, SIGNAL(triggered()),
            mdiArea, SLOT(closeAllSubWindows()));

    tileAct = new QAction(tr("&Tile"), this);
    tileAct->setStatusTip(tr("Tile the windows"));
    connect(tileAct, SIGNAL(triggered()), mdiArea, SLOT(tileSubWindows()));

    cascadeAct = new QAction(tr("&Cascade"), this);
    cascadeAct->setStatusTip(tr("Cascade the windows"));
    connect(cascadeAct, SIGNAL(triggered()), mdiArea, SLOT(cascadeSubWindows()));

    nextAct = new QAction(tr("Ne&xt"), this);
    nextAct->setShortcuts(QKeySequence::NextChild);
    nextAct->setStatusTip(tr("Move the focus to the next window"));
    connect(nextAct, SIGNAL(triggered()),
            mdiArea, SLOT(activateNextSubWindow()));

    previousAct = new QAction(tr("Pre&vious"), this);
    previousAct->setShortcuts(QKeySequence::PreviousChild);
    previousAct->setStatusTip(tr("Move the focus to the previous "
                                 "window"));
    connect(previousAct, SIGNAL(triggered()),
            mdiArea, SLOT(activatePreviousSubWindow()));

    separatorAct = new QAction(this);
    separatorAct->setSeparator(true);

    aboutAct = new QAction(tr("&About"), this);
    aboutAct->setStatusTip(tr("Show the application's About box"));
    connect(aboutAct, SIGNAL(triggered()), this, SLOT(about()));

    aboutQtAct = new QAction(tr("About &Qt"), this);
    aboutQtAct->setStatusTip(tr("Show the Qt library's About box"));
    connect(aboutQtAct, SIGNAL(triggered()), qApp, SLOT(aboutQt()));

    //create spin boxes
    columnSpinBox = new QSpinBox(this);
    rowSpinBox = new QSpinBox(this);
    columnSpinBox->setStatusTip(tr("Change columns count"));
    rowSpinBox->setStatusTip(tr("Change rows count"));
    columnSpinBox->setMinimum(1);
    rowSpinBox->setMinimum(1);
    connect(columnSpinBox, SIGNAL(valueChanged(int)), this, SLOT(columnValueChange(int)));
    connect(rowSpinBox, SIGNAL(valueChanged(int)), this, SLOT(rowValueChange(int)));
}

void MainWindow::createMenus()
{
    fileMenu = menuBar()->addMenu(tr("&File"));
    fileMenu->addAction(newAct);
    fileMenu->addAction(openAct);
    fileMenu->addAction(saveAct);
    fileMenu->addAction(saveAsAct);
    fileMenu->addSeparator();
    QAction *action = fileMenu->addAction(tr("Switch layout direction"));
    connect(action, SIGNAL(triggered()), this, SLOT(switchLayoutDirection()));
    fileMenu->addAction(exitAct);

    editMenu = menuBar()->addMenu(tr("&Edit"));
    editMenu->addAction(cutAct);
    editMenu->addAction(copyAct);
    editMenu->addAction(pasteAct);
    editMenu->addAction(getCompAct);

    windowMenu = menuBar()->addMenu(tr("&Window"));
    updateWindowMenu();
    connect(windowMenu, SIGNAL(aboutToShow()), this, SLOT(updateWindowMenu()));

    menuBar()->addSeparator();

    helpMenu = menuBar()->addMenu(tr("&Help"));
    helpMenu->addAction(aboutAct);
    helpMenu->addAction(aboutQtAct);
}

void MainWindow::createToolBars()
{
    fileToolBar = addToolBar(tr("File"));
    fileToolBar->addAction(newAct);
    fileToolBar->addAction(openAct);
    fileToolBar->addAction(saveAct);
    fileToolBar->addAction(getCompAct);
    fileToolBar->addWidget(columnSpinBox);
    fileToolBar->addWidget(rowSpinBox);



    editToolBar = addToolBar(tr("Edit"));
    editToolBar->addAction(cutAct);
    editToolBar->addAction(copyAct);
    editToolBar->addAction(pasteAct);
}

void MainWindow::createStatusBar()
{
    statusBar()->showMessage(tr("Ready"));
}

void MainWindow::readSettings()
{
    QSettings settings("Yuri_Scherbakov", "Svd_program");
    QPoint pos = settings.value("pos", QPoint(200, 200)).toPoint();
    QSize size = settings.value("size", QSize(400, 400)).toSize();
    move(pos);
    resize(size);
}

void MainWindow::writeSettings()
{
    QSettings settings("Yuri_Scherbakov", "Svd_program");
    settings.setValue("pos", pos());
    settings.setValue("size", size());
}

MdiChild *MainWindow::activeMdiChild()
{
    if (QMdiSubWindow *activeSubWindow = mdiArea->activeSubWindow())
        return qobject_cast<MdiChild *>(activeSubWindow->widget());
    return 0;
}

QMdiSubWindow *MainWindow::findMdiChild(const QString &fileName)
{
    QString canonicalFilePath = QFileInfo(fileName).canonicalFilePath();

    foreach (QMdiSubWindow *window, mdiArea->subWindowList()) {
        MdiChild *mdiChild = qobject_cast<MdiChild *>(window->widget());
        if (mdiChild->currentFile() == canonicalFilePath)
            return window;
    }
    return 0;
}

void MainWindow::switchLayoutDirection()
{
    if (layoutDirection() == Qt::LeftToRight)
        qApp->setLayoutDirection(Qt::RightToLeft);
    else
        qApp->setLayoutDirection(Qt::LeftToRight);
}

void MainWindow::setActiveSubWindow(QWidget *window)
{
    if (!window)
        return;
    mdiArea->setActiveSubWindow(qobject_cast<QMdiSubWindow *>(window));
}
