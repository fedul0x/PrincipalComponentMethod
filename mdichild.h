#ifndef MDICHILD_H
#define MDICHILD_H

#include <QTextEdit>
#include <QTableWidget>
#include "graph.h"

class MdiChild : public QTableWidget
{
    Q_OBJECT

public:
    MdiChild();

    void newFile();
    bool loadFile(const QString &fileName);
    bool save();
    bool saveAs();
    bool saveFile(const QString &fileName);
    void setCurFile(QString curFile);
    void setColumnsTitlesPrefix(QString prefix);
    void setRowsTitlesPrefix(QString prefix);

    QString userFriendlyCurrentFile();
    QString currentFile() { return curFile; }
    Graph *curGraph;

protected:
    void closeEvent(QCloseEvent *event);

private slots:
    void documentWasModified();

public slots:
    void toGraph();
    void setColumnCount(int columns);
    void setRowCount(int rows);
    void copy();
    void paste();

private:
    bool maybeSave();
    void setCurrentFile(const QString &fileName);
    QString strippedName(const QString &fullFileName);
    QString columnsTitlesPrefix;
    QString rowsTitlesPrefix;
    QString curFile;
    bool isUntitled;
    bool isGraph;

};

#endif
